use std::io::{self, BufRead};
use std::collections::{HashMap, BinaryHeap};
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
    fn new(board: Board, g: u8, dir_list: Option<Rc<DirectionList>>, cache: &mut HashMap<BoardState, u8>) -> Self {
        let h = *cache.entry(board.state).or_insert_with(|| board.state.heuristic());
        HeapEntry {
            board,
            h,
            g,
            dir_list,
        }
    }

    fn new_with_bound(board: Board, g: u8, dir_list: Option<Rc<DirectionList>>, cache: &mut HashMap<BoardState, u8>, bound: u8) -> Result<Self, u8> {
        let h = *cache.entry(board.state).or_insert_with(|| board.state.heuristic());
        if h + g > bound {
            return Err(h + g);
        }
        Ok(HeapEntry {
            board,
            h,
            g,
            dir_list,
        })
    }

    fn f(&self) -> u8 {
        self.h + self.g
    }
}

impl std::cmp::PartialOrd for HeapEntry {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

impl std::cmp::Ord for HeapEntry {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.f().cmp(&other.f()).reverse()
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

        let mut cache = HashMap::new();
        let heap_entry = HeapEntry::new(start, 0, None, &mut cache);
        let mut bound = heap_entry.f().max(80);

        let mut dir_list = 'outer: loop {

            let mut next_bound = None;
            let mut heap = BinaryHeap::new();
            heap.push(heap_entry.clone());
            let dir_list = loop {
                let heap_entry = match heap.pop() {
                    Some(heap_entry) => heap_entry,
                    None => {
                        bound = next_bound.unwrap();
                        println!("DEBUG: retry with bound {}", bound);
                        continue 'outer;
                    }
                };
                
                if heap_entry.board.state.complete() {
                    break heap_entry.dir_list;
                }

                let g = heap_entry.g;
                let last_direction = heap_entry.dir_list.as_ref().map(|rc| rc.dir);
                for &dir in &[Direction::Up, Direction::Left, Direction::Down, Direction::Right] {
                    if Some(dir.reverse()) != last_direction {
                        let next_dir_list = Rc::new(DirectionList { dir, parent: heap_entry.dir_list.clone() });
                        let mut next_board = heap_entry.board;
                        if !next_board.move_empty(dir) {
                            continue;
                        }

                        match HeapEntry::new_with_bound(next_board, g + 1, Some(next_dir_list), &mut cache, bound) {
                            Ok(heap_entry) => heap.push(heap_entry),
                            Err(next_bound_cand) => {
                                if let Some(current_next_bound) = next_bound {
                                    if current_next_bound > next_bound_cand {
                                        next_bound = Some(next_bound_cand);
                                    }
                                } else {
                                    next_bound = Some(next_bound_cand);
                                }
                            }
                        }
                    }
                }
            };
            break dir_list;
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
