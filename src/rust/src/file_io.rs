use std::{collections::VecDeque, fs, io::{Error, Read}};

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
    let mut file = fs::File::open(input_filename)
        .map_err(|err| CannotOpenInputFile(err))?;

    if !is_valid_bitmap_magic_number(&mut file) { 
        return Err(InvalidBitmapHeader)
    }

    let bm_header = read_bitmap_header(&mut file)
        .ok_or(InvalidBitmapHeader)?;

    let bm_info_header = read_bitmap_info_header(&mut file)
        .ok_or(InvalidBitmapInfoHeader)?;

    create_image(bm_header, bm_info_header, file)
}

fn create_image(
    bm_header: BitmapHeader, bm_info_header: BitmapInfoHeader, file: fs::File
) -> Result<Image, FileIOError> {
    
    Err(NotImplemented)
}

fn is_valid_bitmap_magic_number(file: &mut fs::File) -> bool {
    let mut first_two_char = [0u8; 2];
    let bytes_read = file.read(&mut first_two_char);
    match bytes_read {
        Err(_) => return false,
        Ok(bytes_read) => if bytes_read < 2 { return false }
    }
    
    first_two_char[0] == BITMAP_MAGIC_NUMBER[0] && 
    first_two_char[1] == BITMAP_MAGIC_NUMBER[1]
}

fn read_bitmap_header(file: &mut fs::File) -> Option<BitmapHeader> {
    BitmapHeader::try_create(
        read_u32(file),
        read_u16(file),
        read_u16(file),
        read_u32(file),
    )
}

fn read_bitmap_info_header(file: &mut fs::File) -> Option<BitmapInfoHeader> {
    BitmapInfoHeader::try_create(
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u16(file),
        read_u16(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
        read_u32(file),
    )
}

fn read_u32(file: &mut fs::File) -> Option<u32> {
    let mut four_bytes = [0u8; 4];
    let bytes_read = file.read(&mut four_bytes)
        .ok()?;

    if bytes_read < 4 { return None }
    
    Some(u32::from_le_bytes(four_bytes))
}

fn read_u16(file: &mut fs::File) -> Option<u16> {
    let mut two_bytes = [0u8; 2];
    let bytes_read = file.read(&mut two_bytes)
        .ok()?;

    if bytes_read < 2 { return None }
    
    Some(u16::from_le_bytes(two_bytes))
}