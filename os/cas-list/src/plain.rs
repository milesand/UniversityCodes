// Just a plain linked list, for benchmarking.

type Link<T> = Option<Box<Node<T>>>;

#[derive(Default)]
pub struct List<T> {
    head: Link<T>,
}

struct Node<T> {
    value: T,
    next: Link<T>,
}

impl<T> List<T> {
    pub fn new() -> Self {
        List { head: None }
    }

    pub fn push(&mut self, value: T) {
        self.head = Some(Box::new(Node {
            value,
            next: self.head.take(),
        }));
    }

    pub fn pop(&mut self) -> Option<T> {
        self.head.take().map(|node| {
            self.head = node.next;
            node.value
        })
    }
}

#[cfg(test)]
mod test {
    use super::List;

    #[test]
    fn works() {
        let mut list = List::new();
        for x in 0..16 {
            list.push(x);
        }
        for x in (8..16).rev() {
            assert_eq!(Some(x), list.pop());
        }
        for x in 16..24 {
            list.push(x);
        }
        for x in (16..24).rev() {
            assert_eq!(Some(x), list.pop());
        }
        for x in (0..8).rev() {
            assert_eq!(Some(x), list.pop());
        }
    }
}
