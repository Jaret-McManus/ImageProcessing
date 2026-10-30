use std::{collections::VecDeque, fs, io::Error};

use crate::data_structures::Image;

use FileIOError::{
    CannotOpenInputFile, 
    InvalidBitmapHeader, 
    NotImplemented};

const BITMAP_MAGIC_NUMBER: [u8; 2] = [0x42, 0x4D];

#[derive(Debug)]
#[allow(dead_code)]
pub enum FileIOError {
    CannotOpenInputFile(Error),
    InvalidBitmapHeader,
    NotImplemented
}

pub fn read_file_to_image(input_filename: String) -> Result<Image, FileIOError> {
    let file_contents = fs::read(input_filename)
        .map_err(|err| CannotOpenInputFile(err))?;
    let mut file_contents = VecDeque::from(file_contents);
    
    
    if !is_valid_bitmap_magic_number(file_contents.pop_front(), file_contents.pop_front()) { 
        return Err(InvalidBitmapHeader)
    }

    return Err(NotImplemented);
}

fn is_valid_bitmap_magic_number(char1: Option<u8>, char2: Option<u8>) -> bool {
    let (Some(char1), Some(char2)) = (char1, char2) else {
        return false
    };
    if char1 != BITMAP_MAGIC_NUMBER[0] || char2 != BITMAP_MAGIC_NUMBER[1] {
        return false;
    }
    return true; 
}