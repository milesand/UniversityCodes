# File Processing

File processing code.

## `fp1`

This one handles a sequential file, allowing insertion, delete, and editing of
records.

Two binaries can be built:
- `create_file` can be built via `cargo build --bin create_file`, which creates
  the binary under `target/debug` or `target/release` depending on optimization
  level. This takes files like `master_in.txt` and `transaction_in.txt` and turn
  them into binary-ish sequential file, usable by the following binary.

- `apply` can be built via `cargo build --bin apply`. This takes files created by
  `create_file` and manages them; Specifically, it applies transactions specified
  in transaction file to the master file.

## `fp2`

This one implements [Extendible hashing](https://en.wikipedia.org/wiki/Extendible_hashing).

`main.c` takes data in `input.csv` and generates an extendible hash file.
