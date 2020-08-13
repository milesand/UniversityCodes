// Comments here are written in assumption you've gone through the racey code before.

use std::sync::Arc;
// AtomicU32 is u32 with atomic operations, Ordering tells compiler & cpu how those operations
// should be ordered. These are important when writing stuff like a lock, but the API requires it anyways.
use std::sync::atomic::{AtomicU32, Ordering::*};

use std::iter;
use std::thread;

const THREADS: u32 = 1000;
const INCR: u32 = 1000;

#[inline(never)]
fn increment_loc(loc: &AtomicU32) {
    // Actually, there's `fetch_add` that just does what we want,
    // and `compare_exchange` which is `compare_and_swap` but with more ordering option.
    // But we'll go with `compare_and_swap` since that seems to be what corresponds to C#'s CompareExchange.
    loop {
        // `Relaxed`, also known as 'Reorder any way you'd like, just make it atomic'
        let old = loc.load(Relaxed);
        let new = old + 1;
        if loc.compare_and_swap(old, new, Relaxed) == old {
            break;
        }
    }
}

fn main() {
    let x = Arc::new(AtomicU32::new(0));

    let handles: Vec<_> = iter::repeat_with(|| {
        let x = Arc::clone(&x);
        thread::spawn(move || {
            for _ in 0..INCR {
                increment_loc(&*x);
            }
        })
    }).take(THREADS as usize).collect();

    for handle in handles {
        handle.join().unwrap();
    }

    println!("{}", Arc::try_unwrap(x).ok().unwrap().into_inner());
}