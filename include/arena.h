/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/arena.h
 * @brief Header file for the Arena (aka Linear) Allocator.
 *
 * This module provides a simple linear memory arena implementation that allows
 * efficient memory allocation and deallocation without frequent calls to malloc
 * and free. The arena allocates memory in large chunks, and allocations are done
 * sequentially, which makes deallocation faster and easier by simply resetting
 * the arena. The arena supports reallocating and checkpoints for memory management.
 */

#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdalign.h>
#include <malloc.h>

// --- Memory Alignment ---

#define LINEAR_MEMORY_ALIGNMENT 8

/**
 * @brief Computes x modulo y, optimized for y being a power of two.
 *
 * @param x The number (or address) to compute the offset from.
 * @param y The alignment value; must be greater than 0.
 * @return The remainder when x is divided by y.
 */
static inline uintptr_t memory_bitwise_offset(uintptr_t x, uintptr_t y) {
    assert(y > 0);
    return x & (y - 1);
}

/**
 * @brief Checks if a number is a power of two.
 *
 * @param x The value to check.
 * @return true if x is a nonzero power of two, false otherwise.
 */
static inline bool memory_is_power_of_two(uintptr_t x) {
    return x != 0 && memory_bitwise_offset(x, x) == 0;
}

/**
 * @brief Computes the number of bytes needed to pad address up to alignment.
 *
 * @param address The current address.
 * @param alignment The alignment boundary (must be a power of two).
 * @return The number of bytes to add to address to achieve alignment.
 */
static inline size_t memory_padding_needed(uintptr_t address, size_t alignment) {
    assert(memory_is_power_of_two(alignment));
    size_t offset = memory_bitwise_offset(address, alignment);
    return (offset != 0) ? alignment - offset : 0;
}

/**
 * @brief Allocates aligned memory using posix_memalign.
 *
 * The returned pointer must be freed using free().
 *
 * @param size The size of the memory block in bytes.
 * @param alignment The alignment boundary (must be a power of two).
 * @return A pointer to the allocated memory, or NULL if allocation fails.
 */
void* memory_aligned_alloc(size_t size, size_t alignment);

// --- Linear Allocator ---

/**
 * @struct Arena
 * @brief Represents the linear memory arena.
 *
 * The `Arena` struct manages a buffer that can be used for memory allocations.
 * Allocations are done sequentially, and the arena can be reset to reclaim all
 * allocated memory. The arena supports reallocating to expand its capacity.
 */
typedef struct Arena {
    uint8_t* buffer; ///< Pointer to the buffer holding the memory.
    size_t capacity; ///< Total capacity of the arena.
    size_t offset; ///< Current allocation offset in the arena.
    size_t last_offset; ///< The last offset before a reset.
} Arena;

/**
 * @struct ArenaCheckpoint
 * @brief Represents a checkpoint for the arena's state.
 *
 * The `ArenaCheckpoint` struct captures the state of the arena at a specific point,
 * allowing the arena to be reverted to this state later.
 */
typedef struct ArenaCheckpoint {
    Arena* arena; ///< Pointer to the arena associated with the checkpoint.
    size_t offset; ///< Offset at the time the checkpoint was created.
    size_t last_offset; ///< Last offset at the time the checkpoint was created.
} ArenaCheckpoint;

/**
 * @brief Creates a new arena with the specified capacity.
 *
 * This function initializes a new `Arena` with a given memory capacity. The arena's
 * memory is allocated in a single contiguous block.
 *
 * @param capacity The total capacity of the arena.
 * @return A pointer to the created `Arena` on success, or NULL on failure.
 */
Arena* arena_create(size_t capacity);

/**
 * @brief Allocates memory from the arena.
 *
 * This function allocates memory from the arena with the given size and alignment.
 * It ensures that the allocated memory is properly aligned, and if there is not enough
 * memory left in the arena, it returns NULL.
 *
 * @param arena Pointer to the arena from which to allocate memory.
 * @param size The size of memory to allocate.
 * @param alignment The alignment of the memory to allocate.
 * @return A pointer to the allocated memory, or NULL if allocation fails.
 */
void* arena_alloc(Arena* arena, size_t size, size_t alignment);

/**
 * @brief Resizes the arena to a new capacity.
 *
 * This function reallocates the arena to a new capacity, copying the existing
 * data to the new buffer. The new capacity must be greater than the current capacity.
 *
 * @param arena Pointer to the arena to resize.
 * @param new_capacity The new capacity for the arena.
 * @param alignment The alignment of the memory to allocate.
 * @return `true` if the reallocation was successful, `false` otherwise.
 */
bool arena_realloc(Arena* arena, size_t new_capacity, size_t alignment);

/**
 * @brief Resets the arena to reclaim all memory.
 *
 * This function resets the arena, discarding all allocated memory and returning
 * the arena to its initial state with no memory allocated.
 *
 * @param arena Pointer to the arena to reset.
 */
void arena_reset(Arena* arena);

/**
 * @brief Frees the memory used by the arena.
 *
 * This function frees the arena's memory and any associated resources. After this
 * call, the arena pointer should not be used.
 *
 * @param arena Pointer to the arena to free.
 */
void arena_free(Arena* arena);

/**
 * @brief Begins a checkpoint for the arena.
 *
 * This function creates a checkpoint of the current state of the arena. The checkpoint
 * can be used later to revert the arena to its previous state.
 *
 * @param arena Pointer to the arena to checkpoint.
 * @return A checkpoint representing the current state of the arena.
 */
ArenaCheckpoint arena_checkpoint_begin(Arena* arena);

/**
 * @brief Ends a checkpoint and reverts the arena to its state at the checkpoint.
 *
 * This function restores the arena to the state captured in the provided checkpoint.
 *
 * @param checkpoint The checkpoint to revert to.
 */
void arena_checkpoint_end(ArenaCheckpoint checkpoint);

/**
 * @brief Returns the amount of memory currently used in the arena.
 *
 * This function returns the number of bytes currently allocated in the arena.
 *
 * @param arena Pointer to the arena.
 * @return The number of bytes currently allocated in the arena.
 */
size_t arena_used(const Arena* arena);

/**
 * @brief Returns the amount of memory remaining in the arena.
 *
 * This function returns the number of bytes remaining in the arena that can
 * still be allocated.
 *
 * @param arena Pointer to the arena.
 * @return The number of bytes remaining in the arena.
 */
size_t arena_remaining(const Arena* arena);

/**
 * @brief Prints debug information about the arena.
 *
 * This function outputs the current state of the arena, including the amount
 * of memory used and remaining.
 *
 * @param arena Pointer to the arena.
 */
void arena_debug(const Arena* arena);

#endif // ARENA_H
