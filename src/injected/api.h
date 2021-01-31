struct API {
    const size_t *api;
    size_t swap_chain_off;

    static size_t find_api(Memory memory);

    static API get(Memory memory);

    size_t renderer() {
        return read_u64(*api + 0x10);
    }

    size_t swap_chain() {
        return read_u64(renderer() + swap_chain_off);
    }
};

