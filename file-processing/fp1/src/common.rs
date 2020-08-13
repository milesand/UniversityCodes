#![allow(dead_code)]

pub const KEY_LEN: usize = 5;
pub const NAME_LEN: usize = 15;
pub const ADDR_LEN: usize = 60;
pub const CAR_LEN: usize = 15;
pub const WORK_LEN: usize = 12;
pub const PHONE_LEN: usize = 9;
pub const CODE_LEN: usize = 1;
pub const FILLER_LEN: usize = 2;

pub const MASTER_RECORD_LEN: usize =
    KEY_LEN + NAME_LEN + ADDR_LEN + CAR_LEN + WORK_LEN + PHONE_LEN + FILLER_LEN;
pub const TRANSACTION_RECORD_LEN: usize = MASTER_RECORD_LEN + CODE_LEN;

pub const KEY_INDEX: usize = 0;
pub const NAME_INDEX: usize = KEY_INDEX + KEY_LEN;
pub const ADDR_INDEX: usize = NAME_INDEX + NAME_LEN;
pub const CAR_INDEX: usize = ADDR_INDEX + ADDR_LEN;
pub const WORK_INDEX: usize = CAR_INDEX + CAR_LEN;
pub const PHONE_INDEX: usize = WORK_INDEX + WORK_LEN;
pub const MASTER_FILLER_INDEX: usize = PHONE_INDEX + PHONE_LEN;
pub const CODE_INDEX: usize = PHONE_INDEX + PHONE_LEN;
pub const TRANSACTION_FILLER_INDEX: usize = CODE_INDEX + CODE_LEN;
