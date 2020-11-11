use crate::search::{decode_imm, decode_pc, find_inst};
use byteorder::*;
use hex_literal::*;

pub struct Memory<'a> {
    pub mem: &'a [u8],
    pub exe: &'a [u8],
}

impl<'a> Memory<'a> {
    pub fn r64(&self, addr: usize) -> usize {
        LE::read_u64(&self.mem[addr..]) as usize
    }

    pub fn f32(&self, addr: usize) -> f32 {
        LE::read_f32(&self.mem[addr..])
    }
}

pub struct State<'a> {
    memory: &'a Memory<'a>,
    location: usize,
    off_items: usize,
    off_layers: usize,
}

impl<'a> State<'a> {
    pub fn new(memory: &'a Memory, start: usize) -> State<'a> {
        // Global state pointer
        let location = decode_pc(
            memory.exe,
            find_inst(memory.exe, &hex!("83 78 0C 05 0F 85"), start) - 15,
        );
        // The offset of items field
        let off_items = decode_imm(
            memory.exe,
            find_inst(memory.exe, &hex!("33 D2 8B 41 28 01"), start) - 7,
        ) as usize;
        let off_layers = decode_imm(
            memory.exe,
            find_inst(memory.exe, &hex!("C6 80 58 44 06 00 01 "), start) - 7,
        ) as usize;
        State {
            memory,
            location,
            off_items,
            off_layers,
        }
    }

    fn ptr(&self) -> usize {
        LE::read_u64(&self.memory.exe[self.location..]) as usize
    }

    pub fn layer(&self, index: u8) -> usize {
        self.memory.r64(self.ptr() + self.off_layers + index as usize * 8)
    }

    pub fn items(&self) -> Items {
        let pointer = self.memory.r64(self.ptr() + self.off_items);
        Items {
            memory: self.memory,
            pointer,
        }
    }
}

pub struct Items<'a> {
    memory: &'a Memory<'a>,
    pointer: usize,
}

impl<'a> Items<'a> {
    pub fn player(&self, index: usize) -> Option<Player> {
        let pointer = self.memory.r64(self.pointer + 8 + index * 8);
        match pointer {
            0 => None,
            _ => Some(Player {
                memory: self.memory,
                pointer,
            }),
        }
    }
}

pub struct Player<'a> {
    memory: &'a Memory<'a>,
    pointer: usize,
}

impl<'a> Player<'a> {
    pub fn position(&self) -> (f32, f32) {
        // "overlay" exists if player is riding something / etc
        let (mut x, mut y) = (0.0, 0.0);
        let mut overlay = self.pointer;
        loop {
            let (_x, _y) = (
                self.memory.f32(overlay + 0x40),
                self.memory.f32(overlay + 0x44),
            );
            x += _x;
            y += _y;
            overlay = self.memory.r64(overlay + 0x10);
            if overlay == 0 {
                break;
            }
        }
        (x, y)
    }

    pub fn layer(&self) -> u8 {
        self.memory.mem[self.pointer + 0x98]
    }
}
