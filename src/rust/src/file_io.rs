use std::{collections::VecDeque, fs, io::Error};

use crate::data_structures::{Image, BitmapInfoHeader};

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

    let _bitmap_info_header = BitmapInfoHeader::try_create(
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u16(&mut file_contents),
        read_u16(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
        read_u32(&mut file_contents),
    ).ok_or(InvalidBitmapHeader)?;

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

fn read_u32(file_contents: &mut VecDeque<u8>) -> Option<u32> {
    let mut char: u8;
    let mut read_value: u32 = 0;
    for i in 0..4 {
        char = file_contents.pop_front()?;
        read_value += (char as u32) << i * 8;
    }

    Some(read_value)
}

fn read_u16(file_contents: &mut VecDeque<u8>) -> Option<u16> {
    let mut char: u8;
    let mut read_value: u16 = 0;
    for i in 0..2 {
        char = file_contents.pop_front()?;
        read_value += (char as u16) << i * 8;
    }

    Some(read_value)
}