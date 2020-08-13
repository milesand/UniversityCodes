// I thought I'd do this with rust because why not?
// So, yeah, rust is all about memory safety and that includes no race conditions.
// We actually want this code to BE racey, so we'll bypass some safety gurantees
// in the `std` with the `unsafe` keyword (A way to tell the compiler that this
// code has been manually checked and can be trusted.(Which, in this case, is a lie.)).
// Do note that most programs can be written without EVER doing the `unsafe` dance
// we are doing in this code.

// Rust doesn't really like shared mutation. The compiler can optimize things
// under the assumption that shared mutation doesn't happen! And indeed, without
// writing `unsafe` code yourself or using safe wrappers in external libraries
// (such as `std`), there's no way to achieve that.
// But when you're writing such safe wrappers, you need a way to tell the compiler that
// some memory location is shared AND mutable. UnsafeCell is a bit of 'magic' type that
// does just that.
// (Actually I'm not 100% sure this is necessary here. I don't know, I'll just use it here.)
use std::cell::UnsafeCell;

// 'Atomically reference counted'. A shared, reference counted smart pointer to some
// heap allocated value. Its cousin `Rc` is actually pretty much the same, but its
// reference counts are not guranteed to be handled atomically. This means race condition
// when trying to change the reference count in multiple threads! So `Rc` should never
// escape the thread it was created on, and if you want that, you should use `Arc`.
// Rust actually manages to turn this into a compiler-check. So if you try to pass `Rc` to
// another thread (without doing the `unsafe` dance), compiler will yell at you.
//
// While we want a race condition, we want a more tame race condition, not something like
// use-after-free. So, Arc it is.
use std::sync::Arc;

// Iterator stuff. Carray on, nothing to see here.
use std::iter;
// Threads. Carry on, nothing to see here.
use std::thread;

// Number of threads to spawn.
const THREADS: u32 = 1000;
// Number of increments to perform per thread.
const INCR: u32 = 1000;

// Oh boy, here we go.
// Remember what I said about `Rc` not being able to cross thread boundaries, because that's
// a compile-time error? Well, rust checks that using what's called a 'trait', which is basically
// an interface/contract a type implements/follows. And then there are 'unsafe trait's, whose contracts
// are REALLY important that if those contracts don't hold up, something might blow up in your face.
// `Sync` and `Send` are unsafe traits. `Sync` means that the type can be shared by multiple threads.
// `Send` means that the type can move across thread boundaries. Indeed, those terms are abstract but
// that's due to my lack of ability to explain. For more information, you might want to read 'The Rust
// Programming Language' available at https://doc.rust-lang.org/book/index.html. I think reading Chapter
// 3.1, 4, and 16.4 should suffice, but maybe not.
// Now, usually these traits are implemented automatically, so you don't have to manually implement them,
// unless you're writing something like `Mutex` yourself, you're doing something evil, or whatever.
// We're doing something evil. We'll implement it manually.
//
// To be specific, `UnsafeCell` is not `Sync` by default, and for `Arc<T>` to be `Send`, `T` must be `Sync`
// and `Send`. We're good with the `Send` part, we need to handle the `Sync` part.
// Since there's a rule that forbids implementing foreign trait for foreign type we can't directly implement
// `Sync` for `UnsafeCell`. Instead we'll wrap it in a new type, and implement `Sync` for that type.
struct AssertSync<T>(T);
unsafe impl<T> Sync for AssertSync<T> {}
// We won't give this type any API, we'll just access the field directly.

// Turns out the compiler optimizes micro-increments-in-loop into a
// single giant increment. We don't want that! For that we turn each
// increment into a non-inlinable function call so the compiler can't
// optimize it.
#[inline(never)]
unsafe fn increment_loc(loc: &UnsafeCell<u32>) {
    *loc.get() += 1;
}

// Main fuction. You already know what that means.
fn main() {
    // Yeah, these might be a bit dense if it's your first time seeing the language.
    // I, uhh, recommend reading the aforementioned book and `std`'s API documentation.
    // Can't quite explain this step by step by myself. Sorry.

    let x = Arc::new(AssertSync(UnsafeCell::new(0)));

    let handles: Vec<_> = iter::repeat_with(|| {
        let x = Arc::clone(&x);
        thread::spawn(move || unsafe {
            for _ in 0..INCR {
                increment_loc(&x.0);
            }
        })
    }).take(THREADS as usize).collect();

    for handle in handles {
        handle.join().unwrap();
    }

    println!("{}", Arc::try_unwrap(x).ok().unwrap().0.into_inner());
}