#[macro_use]
extern crate clap;

use std::ffi::OsStr;
use std::fs::File;
use std::io::prelude::*;
use std::io::{self, BufWriter};

mod common;
use common::*;

macro_rules! read_field {
    ($record:ident, $buf:ident, $input:ident, $output:ident, $main:tt, $len:expr, $idx:expr, $end:expr, $field:expr) => {
        $buf.clear();
        let i = $input
            .by_ref()
            .take(($len as u64) + 1)
            .read_until($end, &mut $buf)?;
        if i == 0 {
            break $main;
        }
        if i == $len + 1 && $buf[i - 1] != $end {
            writeln!(
                $output,
                concat!($field, " was too long (Max {} bytes). Please try again."),
                $len
            )?;
            $output.flush()?;
            continue;
        }
        if $buf.last() == Some(&$end) {
            $buf.pop();
        }
        $record[$idx..($idx + $buf.len())].copy_from_slice(&$buf);
    };
}

#[allow(clippy::cyclomatic_complexity)]
fn main() -> io::Result<()> {
    let args = clap_app!(solution =>
        (version: "0.9")
        (author: "Me <shema7k@gmail.com>")
        (about: "Does what you asked me to do (mostly)")
        (@arg output: -o --output +takes_value "Set output file's path. If not specified, output is saved to ./out.")
        (@group mode !required =>
            (@arg master: -m --master "Create master file. (default)")
            (@arg transaction: -t --transaction "Create transaction file.")
        )
    ).get_matches();

    let transaction_mode = args.is_present("transaction");

    let mut file = BufWriter::new(File::create(
        if let Some(path) = args.value_of_os("output") {
            path
        } else {
            OsStr::new("out")
        },
    )?);

    let stdin = io::stdin();
    let mut stdin = stdin.lock();
    let stdout = io::stdout();
    let mut stdout = stdout.lock();

    writeln!(
        stdout,
        "\
         Enter records in the following format:\n\
         > <key> <name>\n\
         + <address>\n\
         + <car> <work> <contact>{}\n\
         No field other than address may contain spaces.\n\
         Enter EOF (Ctrl-D) to end input.\
         ",
        if transaction_mode { " <code>" } else { "" }
    )?;

    let mut buf = Vec::new();
    'main: loop {
        let mut record = [0; TRANSACTION_RECORD_LEN]; // Actually, should be max(MASTER_RECORD_LEN, TRANSACTION_RECORD_LEN)
        loop {
            write!(stdout, "> ")?;
            stdout.flush()?;

            read_field!(record, buf, stdin, stdout, 'main, KEY_LEN, KEY_INDEX, b' ', "Key");
            read_field!(record, buf, stdin, stdout, 'main, NAME_LEN, NAME_INDEX, b'\n', "Name");

            break;
        }
        loop {
            write!(stdout, "+ ")?;
            stdout.flush()?;

            read_field!(record, buf, stdin, stdout, 'main, ADDR_LEN, ADDR_INDEX, b'\n', "Address");

            break;
        }
        loop {
            write!(stdout, "+ ")?;
            stdout.flush()?;
            read_field!(record, buf, stdin, stdout, 'main, CAR_LEN, CAR_INDEX, b' ', "Car name");
            read_field!(record, buf, stdin, stdout, 'main, WORK_LEN, WORK_INDEX, b' ', "Workplace name");
            read_field!(record, buf, stdin, stdout, 'main, PHONE_LEN, PHONE_INDEX, if transaction_mode { b' ' } else { b'\n' }, "Contact");

            if transaction_mode {
                read_field!(record, buf, stdin, stdout, 'main, CODE_LEN, CODE_INDEX, b'\n', "Transaction code");
            }

            break;
        }
        file.write_all(
            &record[..if transaction_mode {
                TRANSACTION_RECORD_LEN
            } else {
                MASTER_RECORD_LEN
            }],
        )?;
    }

    Ok(())
}
