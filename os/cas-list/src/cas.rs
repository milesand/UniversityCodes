use std::mem::ManuallyDrop;
use std::sync::atomic::Ordering::*;

// This is... Well, this is an external dependency. It handles epoch-based
// memory reclamation, for solving ABA problem.
// Now, ABA happens because athread can pop and deallocate a node, while another
// thread is trying to pop that node. If the node is not deallocated, new node
// cannot occupy that node's location, thus the pushed node in midpop-interrupt-
// pop-pop-push scenario cannot have the same address as the midpop one. ABA averted!
// This also solves use-after-free where we load our pointer to head, but before
// we read the next field in the head, some other thread comes in and pops our head.
// If the popped head is deallocated, we'll try to read from deallocated memory,
// and that is bad.
// TL;DR: If we defer node deallocation to the point where every thread's done popping,
// we can solve ABA and use-after-free. Implementing this from scratch isn't easy
// since it involves even more shared state. So I'll just get away with this.
use crossbeam_epoch as epoch;
use epoch::{Atomic, Owned};

#[derive(Default)]
pub struct List<T> {
    head: Atomic<Node<T>>,
}

struct Node<T> {
    value: ManuallyDrop<T>,
    next: Atomic<Node<T>>,
}

impl<T> List<T> {
    pub fn new() -> Self {
        List {
            head: Atomic::null(),
        }
    }

    pub fn push(&self, value: T) {
        let mut new_node = Owned::new(Node {
            value: ManuallyDrop::new(value),
            next: Atomic::null(),
        });

        // Actually I'm not entirely sure about this. Deallocating mid-push sounds OK.
        // The API allows creating a dummy guard for that too. Buuuut just to be safe...
        let guard = &epoch::pin();
        let mut old_head = self.head.load(Relaxed, guard);

        loop {
            new_node.next.store(old_head, Relaxed);
            // Yeah, they're called `compare_and_set` but they're the compare-and-swap you know.
            // Ok means Swap suceeded, Err means it failed. You also get some context out of these.
            match self
                .head
                .compare_and_set(old_head, new_node, Release, guard)
            {
                Ok(_) => break,
                Err(e) => {
                    new_node = e.new;
                    old_head = e.current;
                }
            }
        }
    }

    pub fn pop(&self) -> Option<T> {
        // Mark that we're popping. All deallocation will be delayed until the point all guards across threads are gone.
        let guard = &epoch::pin();

        // About the memory orderings: I *might* have looked at someone else's code, to see what ordering to use.
        // I'm still kind of mystified by these. Maybe I should've been honest and just gone with SeqCst everywhere.
        let mut old_head = self.head.load(Acquire, guard);
        loop {
            if old_head.is_null() {
                return None;
            }
            let next = unsafe { old_head.deref().next.load(Relaxed, guard) };
            match self
                .head
                .compare_and_set(old_head, next, AcqRel, guard)
            {
                Ok(_) => unsafe {
                    let value = ManuallyDrop::take(&mut old_head.deref_mut().value);
                    guard.defer_destroy(old_head);
                    return Some(value);
                },
                Err(e) => {
                    old_head = e.current;
                }
            }
        }
    }
}

#[cfg(test)]
mod test {
    use super::List;

    #[test]
    fn single_threaded() {
        let list = List::new();
        for x in 0..16 {
            list.push(x);
        }
        for x in (8..16).rev() {
            assert_eq!(Some(x), list.pop());
        }
        for x in 16..24 {
            list.push(x);
        }
        for x in (16..24).rev() {
            assert_eq!(Some(x), list.pop());
        }
        for x in (0..8).rev() {
            assert_eq!(Some(x), list.pop());
        }
    }

    #[test]
    fn multi_threaded() {
        use std::iter;
        use std::sync::atomic::{AtomicBool, Ordering::*};
        use std::sync::Arc;
        use std::thread;

        let list = Arc::new(List::new());
        let record = Arc::new(
            iter::repeat_with(|| AtomicBool::new(false))
                .take(8 * 1024)
                .collect::<Vec<_>>(),
        );
        let handles = (0..8).map(|x| {
            let list = Arc::clone(&list);
            let record = Arc::clone(&record);
            thread::spawn(move || {
                let base = 1024 * x;
                for offset in (0..512).map(|n| 2 * n) {
                    list.push(base + offset);
                    list.push(base + offset + 1);
                    for _ in 0..2 {
                        let idx = list.pop().expect("Pop from an empty list");
                        if record[idx].swap(true, Relaxed) {
                            panic!("Same value popped twice");
                        }
                    }
                }
            })
        });

        for handle in handles {
            handle.join().unwrap();
        }

        assert!(record.iter().all(|ab| ab.load(Relaxed)));
    }
}
