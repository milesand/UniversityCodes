use std::time::{Duration, Instant};

const VALUE: u64 = (1 << 61) - 1;
const ITER: u32 = 200;

fn main() {
    println!("Testing primality of {}", VALUE);
    let mut total = Duration::new(0, 0);
    for i in 1..=ITER {
        println!("\nIteration {}", i);
        let start = Instant::now();
        let res = prime_test(VALUE);
        let time = start.elapsed();
        match res {
            Ok(()) => println!("  {} is prime", VALUE),
            Err(f) => println!("  {} = {} * {}; not prime", VALUE, f, VALUE / f),
        }
        println!("  Finished in {}s {}ns", time.as_secs(), time.subsec_nanos());
        total += time;
    }
    let avg = total / ITER;
    println!("\nAverage time: {}s {}ns", avg.as_secs(), avg.subsec_nanos());
}

fn prime_test(value: u64) -> Result<(), u64> {
    if value & 1 == 0 {
        return Err(2);
    }
    let mut next: Option<(u64, u64)> = Some((3, 9));
    while let Some((n, sq)) = next {
        if sq > value {
            break;
        }
        if value % n == 0 {
            return Err(n);
        }
        next = n
            .checked_add(2)
            .and_then(|n| n.checked_mul(n).map(|sq| (n, sq)));
    }
    Ok(())
}
