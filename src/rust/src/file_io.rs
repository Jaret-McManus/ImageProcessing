use std::{collections::VecDeque, fs, io::Error};

use crate::data_structures::{BitmapHeader, BitmapInfoHeader, Image};

use FileIOError::{
    CannotOpenInputFile, 
    InvalidBitmapHeader,
    InvalidBitmapInfoHeader,
    NotImplemented
};

const BITMAP_MAGIC_NUMBER: [u8; 2] = [0x42, 0x4D];

#[derive(Debug)]
#[allow(dead_code)]
pub enum FileIOError {
    CannotOpenInputFile(Error),
    InvalidBitmapHeader,
    InvalidBitmapInfoHeader,
    NotImplemented
}

pub fn read_file_to_image(input_filename: String) -> Result<Image, FileIOError> {
    let file_contents = fs::read(input_filename)
        .map_err(|err| CannotOpenInputFile(err))?;
    let mut file_contents = VecDeque::from(file_contents);
    
    if !is_valid_bitmap_magic_number(&mut file_contents) { 
        return Err(InvalidBitmapHeader)
    }

    let bm_header = read_bitmap_header(&mut file_contents)
        .ok_or(InvalidBitmapHeader)?;

    let bm_info_header = read_bitmap_info_header(&mut file_contents)
        .ok_or(InvalidBitmapInfoHeader)?;

    create_image(bm_header, bm_info_header, file_contents)
}

fn create_image(
    bm_header: BitmapHeader, bm_info_header: BitmapInfoHeader, file_contents: VecDeque<u8>
) -> Result<Image, FileIOError> {
    Err(NotImplemented)
}

fn is_valid_bitmap_magic_number(file_contents: &mut VecDeque<u8>) -> bool {
    let Some(char1) = file_contents.pop_front() else { return false; };
    let Some(char2) = file_contents.pop_front() else { return false; };
    
    char1 == BITMAP_MAGIC_NUMBER[0] && char2 == BITMAP_MAGIC_NUMBER[1]
}

fn read_bitmap_header(file_contents: &mut VecDeque<u8>) -> Option<BitmapHeader> {
    BitmapHeader::try_create(
        read_u32(file_contents),
        read_u16(file_contents),
        read_u16(file_contents),
        read_u32(file_contents),
    )
}

fn read_bitmap_info_header(file_contents: &mut VecDeque<u8>) -> Option<BitmapInfoHeader> {
    BitmapInfoHeader::try_create(
        read_u32(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
        read_u16(file_contents),
        read_u16(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
        read_u32(file_contents),
    )
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