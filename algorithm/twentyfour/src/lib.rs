use std::rc::Rc;
use std::io::BufRead;

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub enum Direction {
    Up,
    Left,
    Down,
    Right,
}

impl Direction {
    pub fn reverse(self) -> Self {
        use Direction::*;
        match self {
            Up => Down,
            Left => Right,
            Down => Up,
            Right => Left,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DirectionList {
    pub dir: Direction,
    pub parent: Option<Rc<DirectionList>>,
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub struct BoardState(pub [u8; 25]);

impl BoardState {
    pub fn heuristic(&self) -> u8 {
        let mut cost = 0;

        // manhattan distance
        for x in 0..5 {
            for y in 0..5 {
                let idx = 5 * y + x;
                let val = self.0[idx];
                if val != 25 {
                    let x = x as u8;
                    let y = y as u8;
                    let tx = (val - 1) % 5;
                    let ty = (val - 1) / 5;
                    cost += tx.max(x) - tx.min(x);
                    cost += ty.max(y) - ty.min(y);
                }
            }
        }

        // linear conflict
        for i in 0..5 {
            for j in 0..5 {
                let idx = 5 * i + j;
                let val = self.0[idx as usize];
                if val != 25 && 5 * i < val && val <= 5 * i + 5 {
                    for k in (j + 1)..5 {
                        let idx = 5 * i + k;
                        let other = self.0[idx as usize];
                        if other != 25 && 5 * i < other && other <= 5 * i + 5 && val > other {
                            cost += 2;
                        }
                    }
                }
            } 
        }

        for i in 0..5 {
            for j in 0..5 {
                let idx = 5 * i + j;
                let val = self.0[idx as usize];
                if val != 25 && val % 5 == (j + 1) % 5 {
                    for k in (i + 1)..5 {
                        let idx = 5 * k + j;
                        let other = self.0[idx as usize];
                        if other != 25 && other % 5 == (j + 1) % 5 && val > other {
                            cost += 2;
                        }
                    }
                }
            } 
        }

        cost
    }

    pub fn complete(&self) -> bool {
        self.0.iter().enumerate().all(|(i, &val)| i as u8 + 1 == val)
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct Board {
    pub state: BoardState,
    empty: usize,
}

impl Board {
    pub fn parse_from<R: BufRead>(r: &mut R) -> Self {
        let mut buf = Vec::new();
        let mut arr = [0; 25];
        let mut check = [false; 25];
        let mut empty = 0;
        for y in 0..5 {
            for x in 0..5 {
                let idx = 5 * y + x;
                buf.clear();
                r.read_until(if x == 4 { b'\n' } else { b' ' }, &mut buf).unwrap();
                let value = String::from_utf8_lossy(&buf).trim().parse().unwrap();
                arr[idx] = value;
                if value == 25 {
                    empty = idx;
                }
                check[value as usize - 1] = true;
            }
        }

        if check.iter().any(|&b| !b) {
            panic!("Invalid input");
        }

        Board {
            state: BoardState(arr),
            empty,
        }
    }

    pub fn move_empty(&mut self, direction: Direction) -> bool {
        use Direction::{Up, Left, Down, Right};
        let target = match direction {
            Up => self.empty.checked_sub(5),
            Left => if self.empty % 5 == 0 { None } else { Some(self.empty - 1) },
            Down => if self.empty >= 20 { None } else { Some(self.empty + 5) },
            Right => if self.empty % 5 == 4 { None } else { Some(self.empty + 1) },
        };
        if let Some(idx) = target {
            self.state.0.swap(self.empty, idx);
            self.empty = idx;
            true
        } else {
            false
        }
    }

    pub fn solvable(&self) -> bool {
        let mut solvable = true;
        for i in 0..24 {
            if self.state.0[i] == 25 {
                continue;
            }
            for j in (i + 1)..25 {
                if self.state.0[j] == 25 {
                    continue;
                }
                if self.state.0[i] > self.state.0[j] {
                    solvable = !solvable;
                }
            }
        }
        solvable
    }
}