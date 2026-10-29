use std::{collections::VecDeque, env};


fn main() {
    println!("Hello, world!");
    let mut args: VecDeque<String> = env::args().collect();
    args.pop_front();
    println!("{args:?}");
}
