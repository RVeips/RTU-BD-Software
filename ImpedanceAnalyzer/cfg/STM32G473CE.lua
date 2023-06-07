return {
    EntryPoint = "__cfxs_reset",
    Memory = {
        ["FLASH"] = { permissions = "r-x", start = 0x08000000, size = 256 * kB },
        ["RAM"]   = { permissions = "rwx", start = 0x20000000, size = 128 * kB },
        Alias     = {}
    },
    Stack = {
        location = "RAM",
        size = 2 * kB
    },
    -- InsertBefore("RAM", {
    --     Define("__RAM_VECTOR_TABLE_START__"),
    --     Extend(0x200),
    --     Define("__RAM_VECTOR_TABLE_END__"),
    -- }),
    NoDefaultSections = false,
    Sections = {
        {
            name = "heap",
            location = "RAM",
            target = "RAM",
            type = NO_LOAD,
            content = {
                Define("__HEAP_START__"),
                Extend(4 * kB),
                Define("__HEAP_END__"),
            }
        },
        {
            name = "stack",
            location = "RAM",
            target = "RAM",
            type = NO_LOAD,
            content = {
                Align(8),
                Define("__STACK_PROTECT__"),
                Extend(64),
                Define("__STACK_END__"),
                Extend(2 * kB),
                Define("__STACK_START__"),
            }
        },
    },
    -- Discard = {
    --     ["libc.a"] = {},
    --     ["libm.a"] = {},
    --     ["libgcc.a"] = {},
    -- }
}
