use byteorder::*;
use twoway::find_bytes;

pub fn decode_pc(exe: &[u8], offset: usize) -> usize {
    let rel = LE::read_i32(&exe[offset + 3..]) as usize;
    offset.wrapping_add(rel + 7)
}

pub fn decode_imm(exe: &[u8], offset: usize) -> usize {
    LE::read_u32(&exe[offset + 3..]) as usize
}

pub fn find_after_bundle(exe: &[u8]) -> usize {
    let mut offset = 0x1000;

    loop {
        let (l0, l1) = (
            LE::read_u32(&exe[offset..]),
            LE::read_u32(&exe[offset + 4..]),
        );
        if l0 == 0 && l1 == 0 {
            break;
        }
        offset += (8 + l0 + l1) as usize;
    }

    find_inst(exe, &[0x48, 0x81, 0xEC, 0xE8, 0x00, 0x00, 0x00], offset)
}

pub fn find_inst(exe: &[u8], needle: &[u8], start: usize) -> usize {
    // Find the location of the instruction (needle) using memmem()
    match find_bytes(&exe[start..], needle) {
        Some(offset) => offset + start,
        None => panic!("Instruction not found!"),
    }
}
