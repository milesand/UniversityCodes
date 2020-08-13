// Some nightly stuff! test is for benchmarking, manually_drop_take is for some function.
#![feature(test)]
#![feature(manually_drop_take)]

pub mod cas;
pub mod plain;

#[cfg(test)]
mod test {
    extern crate test;

    const THREADS: usize = 8;
    const ITERS: usize = 512;

    use test::Bencher;

    use std::sync::{Arc, Mutex};
    use std::thread;

    // So here's the gist.
    // - 8 Threads are spawned.
    // - Each thread pushes and pops 1024 distinct values into and from a shared list.
    // - Check is performed to see if all values are pushed and popped exactly once.
    // Not sure if this is a good or even an ok benchmark.

    #[bench]
    fn cas(b: &mut Bencher) {
        b.iter(|| {
            use std::iter;
            use std::sync::atomic::{AtomicBool, Ordering::*};

            use crate::cas::List;

            let threads = test::black_box(THREADS);
            let iters = test::black_box(ITERS);

            let list = Arc::new(List::new());
            let record = Arc::new(
                iter::repeat_with(|| AtomicBool::new(false))
                    .take(threads * iters * 2)
                    .collect::<Vec<_>>(),
            );
            let handles = (0..threads).map(|x| {
                let list = Arc::clone(&list);
                let record = Arc::clone(&record);
                thread::spawn(move || {
                    let base = 2 * iters * x;
                    for offset in (0..iters).map(|n| 2 * n) {
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
        });
    }

    #[bench]
    fn mutex_hold_on(b: &mut Bencher) {
        b.iter(|| {
            use std::iter;
            use std::sync::atomic::{AtomicBool, Ordering::*};

            use crate::plain::List;

            let threads = test::black_box(THREADS);
            let iters = test::black_box(ITERS);

            let list = Arc::new(Mutex::new(List::new()));
            let record = Arc::new(
                iter::repeat_with(|| AtomicBool::new(false))
                    .take(threads * iters * 2)
                    .collect::<Vec<_>>(),
            );
            let handles = (0..threads).map(|x| {
                let list = Arc::clone(&list);
                let record = Arc::clone(&record);
                thread::spawn(move || {
                    let base = 2 * iters * x;
                    let mut list = list.lock().unwrap();
                    for offset in (0..iters).map(|n| 2 * n) {
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
        });
    }

    #[bench]
    fn mutex_let_go(b: &mut Bencher) {
        b.iter(|| {
            use std::iter;
            use std::sync::atomic::{AtomicBool, Ordering::*};

            use crate::plain::List;

            let threads = test::black_box(THREADS);
            let iters = test::black_box(ITERS);

            let list = Arc::new(Mutex::new(List::new()));
            let record = Arc::new(
                iter::repeat_with(|| AtomicBool::new(false))
                    .take(threads * iters * 2)
                    .collect::<Vec<_>>(),
            );
            let handles = (0..threads).map(|x| {
                let list = Arc::clone(&list);
                let record = Arc::clone(&record);
                thread::spawn(move || {
                    let base = 2 * iters * x;
                    for offset in (0..iters).map(|n| 2 * n) {
                        list.lock().unwrap().push(base + offset);
                        list.lock().unwrap().push(base + offset + 1);
                        for _ in 0..2 {
                            let idx = list.lock().unwrap().pop().expect("Pop from an empty list");
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
        });
    }
}
