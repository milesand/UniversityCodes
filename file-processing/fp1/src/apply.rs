#[macro_use]
extern crate clap;

use std::fs::File;
use std::io::prelude::*;
use std::io::{self, BufReader, BufWriter};
use tempfile::NamedTempFile;

mod common;
use common::*;

fn main() -> io::Result<()> {
    let args = clap_app!(solution =>
        (version: "0.9")
        (author: "Me <shema7k@gmail.com>")
        (about: "Does what you asked me to do (mostly)")
        (@arg master: -m --master +takes_value +required "Master file to be updated.")
        (@arg transaction: -t --transaction +takes_value +required "Transaction file to be applied.")
        (@arg output: -o --output +takes_value "Set new master file's path. If not specified, output will overwrite master file.")
    ).get_matches();

    let master_path = args.value_of_os("master").unwrap();
    let trans_path = args.value_of_os("transaction").unwrap();
    let output_path = if let Some(output_path) = args.value_of_os("output") {
        output_path
    } else {
        master_path
    };

    let mut out = BufWriter::new(NamedTempFile::new()?);
    let stdout = io::stdout();
    let mut stdout = stdout.lock();
    let stderr = io::stderr();
    let mut stderr = stderr.lock();

    let mut master = BufReader::new(File::open(master_path)?);
    let mut trans = BufReader::new(File::open(trans_path)?);

    let mut master_buf = None;
    let mut trans_buf = [0; TRANSACTION_RECORD_LEN];

    let mut trans_number = 0u64;

    while read_record(&mut trans, &mut trans_buf)? {
        trans_number += 1;
        match trans_buf[CODE_INDEX] {
            0 | 6 | b'0' | b'6' => insert_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            1 | b'1' => update_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                (CAR_INDEX, CAR_LEN),
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            2 | b'2' => delete_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            3 | b'3' => update_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                (ADDR_INDEX, ADDR_LEN),
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            4 | b'4' => update_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                (PHONE_INDEX, PHONE_LEN),
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            5 | b'5' => update_record(
                &mut master,
                &mut master_buf,
                trans_number,
                trans_buf,
                (WORK_INDEX, WORK_LEN),
                &mut out,
                &mut stdout,
                &mut stderr,
            )?,
            unknown => {
                writeln!(
                    stderr,
                    "Error: Unknown transaction code {} at transaction {}",
                    unknown, trans_number,
                )?;
            }
        }
    }

    if let Some(ref mut buf) = master_buf {
        loop {
            out.write_all(&*buf)?;
            pretty_print(&mut stdout, &*buf)?;
            if !read_record(&mut master, buf)? {
                break;
            }
        }
    }

    drop(master);
    drop(trans);
    out.into_inner()?.persist(output_path)?;

    Ok(())
}

fn read_record<R: Read>(r: &mut R, mut buf: &mut [u8]) -> io::Result<bool> {
    let mut read = false;
    while !buf.is_empty() {
        match r.read(buf) {
            Ok(0) => {
                return if read {
                    Err(io::Error::from(io::ErrorKind::UnexpectedEof))
                } else {
                    Ok(false)
                };
            }
            Ok(n) => {
                read = true;
                buf = &mut buf[n..];
            }
            Err(ref e) if e.kind() == io::ErrorKind::Interrupted => continue,
            Err(e) => return Err(e),
        }
    }
    Ok(true)
}

fn insert_record<R, W1, W2, W3>(
    master_file: &mut R,
    master_buf: &mut Option<[u8; MASTER_RECORD_LEN]>,
    transaction_number: u64,
    transaction: [u8; TRANSACTION_RECORD_LEN],
    output: &mut W1,
    digest: &mut W2,
    error: &mut W3,
) -> io::Result<()>
where
    R: Read + Seek,
    W1: Write,
    W2: Write,
    W3: Write,
{
    skip_records(master_file, master_buf, transaction, output, digest)?;
    match *master_buf {
        None => {
            let mut buf = [0; MASTER_RECORD_LEN];
            buf[..CODE_INDEX].copy_from_slice(&transaction[..CODE_INDEX]);
            *master_buf = Some(buf);
        }
        Some(ref mut buf) => {
            if buf[KEY_INDEX..(KEY_INDEX + KEY_LEN)]
                == transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)]
            {
                writeln!(
                    error,
                    "Error: Insert with already existing key {} at transaction {}",
                    String::from_utf8_lossy(&transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)]),
                    transaction_number,
                )?;
            } else {
                master_file.seek(io::SeekFrom::Current(-(MASTER_RECORD_LEN as i64)))?;
                buf[..CODE_INDEX].copy_from_slice(&transaction[..CODE_INDEX]);
            }
        }
    }
    Ok(())
}

#[allow(clippy::too_many_arguments)]
fn update_record<R, W1, W2, W3>(
    master_file: &mut R,
    master_buf: &mut Option<[u8; MASTER_RECORD_LEN]>,
    transaction_number: u64,
    transaction: [u8; TRANSACTION_RECORD_LEN],
    edit_region: (usize, usize),
    output: &mut W1,
    digest: &mut W2,
    error: &mut W3,
) -> io::Result<()>
where
    R: Read + Seek,
    W1: Write,
    W2: Write,
    W3: Write,
{
    skip_records(master_file, master_buf, transaction, output, digest)?;
    match *master_buf {
        Some(ref mut buf)
            if buf[KEY_INDEX..(KEY_INDEX + KEY_LEN)]
                == transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)] =>
        {
            let start = edit_region.0;
            let end = edit_region.0 + edit_region.1;
            if buf[..start] != transaction[..start]
                || buf[end..MASTER_FILLER_INDEX] != transaction[end..CODE_INDEX]
            {
                writeln!(
                    error,
                    "Error: Update with non-matching field(s) at transaction {}",
                    transaction_number,
                )?;
            } else {
                buf[start..end].copy_from_slice(&transaction[start..end]);
            }
        }
        _ => {
            writeln!(
                error,
                "Error: Update with non-existing key {} at transaction {}",
                String::from_utf8_lossy(&transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)]),
                transaction_number,
            )?;
        }
    }
    Ok(())
}

fn delete_record<R, W1, W2, W3>(
    master_file: &mut R,
    master_buf: &mut Option<[u8; MASTER_RECORD_LEN]>,
    transaction_number: u64,
    transaction: [u8; TRANSACTION_RECORD_LEN],
    output: &mut W1,
    digest: &mut W2,
    error: &mut W3,
) -> io::Result<()>
where
    R: Read + Seek,
    W1: Write,
    W2: Write,
    W3: Write,
{
    skip_records(master_file, master_buf, transaction, output, digest)?;
    match *master_buf {
        Some(ref mut buf)
            if buf[KEY_INDEX..(KEY_INDEX + KEY_LEN)]
                == transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)] =>
        {   
            if buf[..MASTER_FILLER_INDEX] == transaction[..CODE_INDEX] {
                *master_buf = None;
            } else {
                writeln!(
                    error,
                    "Error: Delete with non-matching field(s) at transaction {}",
                    transaction_number,
                )?;
            }
        }
        _ => {
            writeln!(
                error,
                "Error: Delete with non-existing key {} at transaction {}",
                String::from_utf8_lossy(&transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)]),
                transaction_number,
            )?;
        }
    }
    Ok(())
}

fn skip_records<R, W1, W2>(
    master_file: &mut R,
    master_buf: &mut Option<[u8; MASTER_RECORD_LEN]>,
    transaction: [u8; TRANSACTION_RECORD_LEN],
    output: &mut W1,
    digest: &mut W2,
) -> io::Result<()>
where
    R: Read + Seek,
    W1: Write,
    W2: Write,
{
    let buf = match *master_buf {
        None => {
            let mut buf = [0; MASTER_RECORD_LEN];
            if !read_record(master_file, &mut buf)? {
                return Ok(());
            }
            *master_buf = Some(buf);
            master_buf.as_mut().unwrap()
        }
        Some(ref mut buf) => buf,
    };
    while buf[KEY_INDEX..(KEY_INDEX + KEY_LEN)] < transaction[KEY_INDEX..(KEY_INDEX + KEY_LEN)] {
        output.write_all(&*buf)?;
        pretty_print(digest, &*buf)?;
        if !read_record(master_file, buf)? {
            *master_buf = None;
            return Ok(());
        }
    }
    Ok(())
}

fn pretty_print<W: Write>(digest: &mut W, record: &[u8; MASTER_RECORD_LEN]) -> io::Result<()> {
    writeln!(
        digest,
        "{} {} {} {} {} {}",
        String::from_utf8_lossy(&record[KEY_INDEX..(KEY_INDEX + KEY_LEN)]).trim_end_matches('\0'),
        String::from_utf8_lossy(&record[NAME_INDEX..(NAME_INDEX + NAME_LEN)])
            .trim_end_matches('\0'),
        String::from_utf8_lossy(&record[ADDR_INDEX..(ADDR_INDEX + ADDR_LEN)])
            .trim_end_matches('\0'),
        String::from_utf8_lossy(&record[CAR_INDEX..(CAR_INDEX + CAR_LEN)]).trim_end_matches('\0'),
        String::from_utf8_lossy(&record[WORK_INDEX..(WORK_INDEX + WORK_LEN)])
            .trim_end_matches('\0'),
        String::from_utf8_lossy(&record[PHONE_INDEX..(PHONE_INDEX + PHONE_LEN)])
            .trim_end_matches('\0'),
    )
}
