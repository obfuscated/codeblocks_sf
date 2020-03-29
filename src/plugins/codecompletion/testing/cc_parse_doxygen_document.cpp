#include <stdio>

enum ENUM {
    VAR1 = 0, ///< Var1 \sa ENUM
    VAR2, /**< Var2 \sa ENUM */
    /** Var3 \sa ENUM */
    VAR3,
    /// Var4 \sa ENUM
    VAR4
};

#define DEF1 1 ///< Def1
#define DEF2 1 /**< Def2 */
/** Def3 */
#define DEF3 1
/// Def4
#define DEF4 1

int main(int argc, char* argv[]) {
    puts("Hello World!");
    return 0;
}

#define MASK_A  0x30000000          /*!< Mask A descr. */
#define MASK_B  0x00070000          /*!< Mask B descr. */

// VAR               ///< \sa ENUM
// VAR1    // VAR1   ///< Var1
// VAR2              ///< Var2
// VAR3              ///< Var3
// VAR4              ///< Var4
// DEF1              ///< Def1
// DEF2              ///< Def2
// DEF3              ///< Def3
// DEF4              ///< Def4
// MASK_A            ///< Mask A descr
// MASK_B            ///< Mask B descr
// DEF1              ///< -