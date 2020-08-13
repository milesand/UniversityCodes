use std::io::{self, BufRead};
use std::collections::BinaryHeap;
use std::rc::Rc;

use twentyfour::*;

#[derive(Debug, Clone, PartialEq, Eq)]
struct HeapEntry {
    board: Board,
    h: u8, // heuristic cost from this state to goal state. 
    g: u8, // cost from start state to this state.
    // f = h + g
    dir_list: Option<Rc<DirectionList>>,
}

impl HeapEntry {
    fn new(board: Board, g: u8, dir_list: Option<Rc<DirectionList>>) -> Self {
        let h = board.state.heuristic();
        HeapEntry {
            board,
            h,
            g,
            dir_list,
        }
    }
}

impl std::cmp::PartialOrd for HeapEntry {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl std::cmp::Ord for HeapEntry {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        (self.h + self.g).cmp(&(other.h + other.g)).reverse()
    }
}

fn main() {
    let stdin = io::stdin();
    let mut stdin = stdin.lock();

    let ts = {
        let mut buf = String::new();
        stdin.read_line(&mut buf).unwrap();
        buf.trim().parse::<u64>().unwrap()
    };

    for t in 1..=ts {
        print!("#{}: ", t);
        let start = Board::parse_from(&mut stdin);

        if !start.solvable() {
            println!("-1");
            continue;
        }

        let mut heap = BinaryHeap::new();
        heap.push(HeapEntry::new(start, 0, None));
        let mut dir_list = loop {
            let heap_entry = heap.pop().unwrap();
            
            if heap_entry.board.state.complete() {
                break heap_entry.dir_list;
            }

            let g = heap_entry.g;
            let last_direction = heap_entry.dir_list.as_ref().map(|rc| rc.dir);
            for &dir in &[Direction::Up, Direction::Left, Direction::Down, Direction::Right] {
                if Some(dir.reverse()) != last_direction {
                    let next_dir_list = Rc::new(DirectionList { dir, parent: heap_entry.dir_list.clone() });
                    let mut next_board = heap_entry.board;
                    next_board.move_empty(dir);
                    heap.push(HeapEntry::new(next_board, g + 1, Some(next_dir_list)));
                }
            }
        };

        let mut stack = Vec::new();
        while let Some(rc_dir_list) = dir_list {
            stack.push(rc_dir_list.dir);
            dir_list = rc_dir_list.parent.clone();
        }
        print!("{}", stack.len());
        while let Some(dir) = stack.pop() {
            print!(" {}", match dir {
                Direction::Up => "U",
                Direction::Left => "L",
                Direction::Down => "D",
                Direction::Right => "R",
            });
        }
        println!();
    }
}
