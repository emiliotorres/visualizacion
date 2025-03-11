#ifndef CHAR_DATA_H
#define CHAR_DATA_H

typedef enum Char {
CHAR_ALPHA = 1, /* \alpha */
CHAR_AMPERSAND = 2, /* \& */
CHAR_APOSTROPHE = 3, /* ' */
CHAR_ASTERISK = 4, /* \ast */
CHAR_ASTERISK_SYMBOL = 5, /* * */
CHAR_AT = 6, /* @ */
CHAR_BACKSLASH = 7, /* \textbackslash */
CHAR_BACKTICK = 8, /* ` */
CHAR_BETA = 9, /* \beta */
CHAR_BULLET = 10, /* \bullet */
CHAR_CAP = 11, /* \cap */
CHAR_CASES = 12, /* \begin{cases}\phantom{a}\\{}\phantom{b}\end{cases} */
CHAR_CHI = 13, /* \chi */
CHAR_CIRCLE = 14, /* \circ */
CHAR_COLON = 15, /* : */
CHAR_COMMA = 16, /* , */
CHAR_CUP = 17, /* \cup */
CHAR_DELTA = 18, /* \delta */
CHAR_DELTA_UPPER = 19, /* \Delta */
CHAR_DIGIT_0 = 20, /* 0 */
CHAR_DIGIT_1 = 21, /* 1 */
CHAR_DIGIT_2 = 22, /* 2 */
CHAR_DIGIT_3 = 23, /* 3 */
CHAR_DIGIT_4 = 24, /* 4 */
CHAR_DIGIT_5 = 25, /* 5 */
CHAR_DIGIT_6 = 26, /* 6 */
CHAR_DIGIT_7 = 27, /* 7 */
CHAR_DIGIT_8 = 28, /* 8 */
CHAR_DIGIT_9 = 29, /* 9 */
CHAR_DOLLAR = 30, /* \$ */
CHAR_DOUBLE_DOWN_ARROW = 31, /* \Downarrow */
CHAR_DOUBLE_LEFT_ARROW = 32, /* \Leftarrow */
CHAR_DOUBLE_RIGHT_ARROW = 33, /* \Rightarrow */
CHAR_DOUBLE_UP_ARROW = 34, /* \Uparrow */
CHAR_DOWN_ARROW = 35, /* \downarrow */
CHAR_EM_DASH = 36, /* — */
CHAR_EN_DASH = 37, /* – */
CHAR_EPSILON = 38, /* \epsilon */
CHAR_EQUALS = 39, /* = */
CHAR_EQUIV = 40, /* \equiv */
CHAR_ETA = 41, /* \eta */
CHAR_EXCLAMATION = 42, /* ! */
CHAR_EXISTS = 43, /* \exists */
CHAR_FEMININE_ORDINAL = 44, /* ª */
CHAR_FORALL = 45, /* \forall */
CHAR_GAMMA = 46, /* \gamma */
CHAR_GAMMA_UPPER = 47, /* \Gamma */
CHAR_GEQ = 48, /* \geq */
CHAR_GREATER_THAN = 49, /* > */
CHAR_HASH = 50, /* \# */
CHAR_HOOK_RIGHT_ARROW = 51, /* \hookrightarrow */
CHAR_HYPHEN_MINUS = 52, /* - */
CHAR_IFF = 53, /* \iff */
CHAR_IN = 54, /* \in */
CHAR_INFINITY = 55, /* \infty */
CHAR_INT = 56, /* \int */
CHAR_INVERTED_EXCLAMATION = 57, /* ¡ */
CHAR_INVERTED_QUESTION = 58, /* ¿ */
CHAR_IOTA = 59, /* \iota */
CHAR_KAPPA = 60, /* \kappa */
CHAR_LAMBDA = 61, /* \lambda */
CHAR_LAMBDA_UPPER = 62, /* \Lambda */
CHAR_LAND = 63, /* \land */
CHAR_LEFT_ARROW = 64, /* \leftarrow */
CHAR_LEFT_BRACKET = 65, /* [ */
CHAR_LEFT_CURLY = 66, /* \{ */
CHAR_LEFT_GUILLEMET = 67, /* « */
CHAR_LEFT_PAREN = 68, /* ( */
CHAR_LEQ = 69, /* \leq */
CHAR_LESS_THAN = 70, /* < */
CHAR_LOR = 71, /* \lor */
CHAR_LOWER_A = 72, /* a */
CHAR_LOWER_A_TITLE = 73, /* á */
CHAR_LOWER_B = 74, /* b */
CHAR_LOWER_C = 75, /* c */
CHAR_LOWER_C_CEDILLA = 76, /* \c{c} */
CHAR_LOWER_D = 77, /* d */
CHAR_LOWER_E = 78, /* e */
CHAR_LOWER_E_TITLE = 79, /* é */
CHAR_LOWER_F = 80, /* f */
CHAR_LOWER_G = 81, /* g */
CHAR_LOWER_H = 82, /* h */
CHAR_LOWER_I = 83, /* i */
CHAR_LOWER_I_TITLE = 84, /* í */
CHAR_LOWER_J = 85, /* j */
CHAR_LOWER_K = 86, /* k */
CHAR_LOWER_L = 87, /* l */
CHAR_LOWER_M = 88, /* m */
CHAR_LOWER_N = 89, /* n */
CHAR_LOWER_N_TITLE = 90, /* ñ */
CHAR_LOWER_O = 91, /* o */
CHAR_LOWER_O_TITLE = 92, /* ó */
CHAR_LOWER_P = 93, /* p */
CHAR_LOWER_Q = 94, /* q */
CHAR_LOWER_R = 95, /* r */
CHAR_LOWER_S = 96, /* s */
CHAR_LOWER_T = 97, /* t */
CHAR_LOWER_U = 98, /* u */
CHAR_LOWER_U_DIERESIS = 99, /* ü */
CHAR_LOWER_U_TITLE = 100, /* ú */
CHAR_LOWER_V = 101, /* v */
CHAR_LOWER_W = 102, /* w */
CHAR_LOWER_X = 103, /* x */
CHAR_LOWER_Y = 104, /* y */
CHAR_LOWER_Z = 105, /* z */
CHAR_MAPSTO = 106, /* \mapsto */
CHAR_MASCULINE_ORDINAL = 107, /* º */
CHAR_MID = 108, /* \mid */
CHAR_MU = 109, /* \mu */
CHAR_NABLA = 110, /* \nabla */
CHAR_NEG = 111, /* \neg */
CHAR_NEQ = 112, /* \neq */
CHAR_NI = 113, /* \ni */
CHAR_NU = 114, /* \nu */
CHAR_OMEGA = 115, /* \omega */
CHAR_OMEGA_UPPER = 116, /* \Omega */
CHAR_PARTIAL = 117, /* \partial */
CHAR_PERCENT = 118, /* \% */
CHAR_PERIOD = 119, /* . */
CHAR_PERP = 120, /* \perp */
CHAR_PHI = 121, /* \phi */
CHAR_PHI_UPPER = 122, /* \Phi */
CHAR_PI = 123, /* \pi */
CHAR_PI_UPPER = 124, /* \Pi */
CHAR_PLUS = 125, /* + */
CHAR_PREC = 126, /* \prec */
CHAR_PRECEQ = 127, /* \preceq */
CHAR_PROD = 128, /* \prod */
CHAR_PROPTO = 129, /* \propto */
CHAR_PSI = 130, /* \psi */
CHAR_PSI_UPPER = 131, /* \Psi */
CHAR_QUESTION = 132, /* ? */
CHAR_QUOTE = 133, /* " */
CHAR_RHO = 134, /* \rho */
CHAR_RIGHT_ARROW = 135, /* \rightarrow */
CHAR_RIGHT_BRACKET = 136, /* ] */
CHAR_RIGHT_CURLY = 137, /* \} */
CHAR_RIGHT_GUILLEMET = 138, /* » */
CHAR_RIGHT_PAREN = 139, /* ) */
CHAR_SEMICOLON = 140, /* ; */
CHAR_SIGMA = 141, /* \sigma */
CHAR_SIGMA_UPPER = 142, /* \Sigma */
CHAR_SIM = 143, /* \sim */
CHAR_SIMEQ = 144, /* \simeq */
CHAR_SLASH = 145, /* / */
CHAR_SQUARE = 146, /* \square */
CHAR_STAR = 147, /* \star */
CHAR_SUBSET = 148, /* \subset */
CHAR_SUM = 149, /* \sum */
CHAR_SUPSET = 150, /* \supset */
CHAR_TAU = 151, /* \tau */
CHAR_THETA = 152, /* \theta */
CHAR_THETA_UPPER = 153, /* \Theta */
CHAR_TIMES = 154, /* \times */
CHAR_UP_ARROW = 155, /* \uparrow */
CHAR_UPPER_A = 156, /* A */
CHAR_UPPER_A_TITLE = 157, /* Á */
CHAR_UPPER_B = 158, /* B */
CHAR_UPPER_C = 159, /* C */
CHAR_UPPER_C_CEDILLA = 160, /* \c{C} */
CHAR_UPPER_D = 161, /* D */
CHAR_UPPER_E = 162, /* E */
CHAR_UPPER_E_TITLE = 163, /* É */
CHAR_UPPER_F = 164, /* F */
CHAR_UPPER_G = 165, /* G */
CHAR_UPPER_H = 166, /* H */
CHAR_UPPER_I = 167, /* I */
CHAR_UPPER_I_TITLE = 168, /* Í */
CHAR_UPPER_J = 169, /* J */
CHAR_UPPER_K = 170, /* K */
CHAR_UPPER_L = 171, /* L */
CHAR_UPPER_M = 172, /* M */
CHAR_UPPER_N = 173, /* N */
CHAR_UPPER_N_TITLE = 174, /* Ñ */
CHAR_UPPER_O = 175, /* O */
CHAR_UPPER_O_TITLE = 176, /* Ó */
CHAR_UPPER_P = 177, /* P */
CHAR_UPPER_Q = 178, /* Q */
CHAR_UPPER_R = 179, /* R */
CHAR_UPPER_S = 180, /* S */
CHAR_UPPER_T = 181, /* T */
CHAR_UPPER_U = 182, /* U */
CHAR_UPPER_U_DIERESIS = 183, /* Ü */
CHAR_UPPER_U_TITLE = 184, /* Ú */
CHAR_UPPER_V = 185, /* V */
CHAR_UPPER_W = 186, /* W */
CHAR_UPPER_X = 187, /* X */
CHAR_UPPER_Y = 188, /* Y */
CHAR_UPPER_Z = 189, /* Z */
CHAR_UPSILON = 190, /* \upsilon */
CHAR_UPSILON_UPPER = 191, /* \Upsilon */
CHAR_XI = 192, /* \xi */
CHAR_XI_UPPER = 193, /* \Xi */
CHAR_ZETA = 194 /* \zeta */
} Char;

typedef enum Style {
BFIT = 1,
CURSIVA = 2,
MATHEQNORMAL = 3,
MATHEQSUB = 4,
MATHTXNORMAL = 5,
MATHTXSUB = 6,
NEGRITA = 7,
NORMAL = 8
} Style;

const Char CharsWithVariousSegments[] ={
CHAR_COLON,
CHAR_EQUALS,
CHAR_EQUIV,
CHAR_EXCLAMATION,
CHAR_FEMININE_ORDINAL,
CHAR_GEQ,
CHAR_INVERTED_EXCLAMATION,
CHAR_INVERTED_QUESTION,
CHAR_LEFT_GUILLEMET,
CHAR_LEQ,
CHAR_LOWER_A_TITLE,
CHAR_LOWER_E_TITLE,
CHAR_LOWER_I,
CHAR_LOWER_I_TITLE,
CHAR_LOWER_J,
CHAR_LOWER_N_TITLE,
CHAR_LOWER_O_TITLE,
CHAR_LOWER_U_DIERESIS,
CHAR_LOWER_U_TITLE,
CHAR_MASCULINE_ORDINAL,
CHAR_PERCENT,
CHAR_PRECEQ,
CHAR_QUESTION,
CHAR_QUOTE,
CHAR_RIGHT_GUILLEMET,
CHAR_SEMICOLON,
CHAR_SIMEQ,
CHAR_THETA_UPPER,
CHAR_UPPER_A_TITLE,
CHAR_UPPER_E_TITLE,
CHAR_UPPER_I_TITLE,
CHAR_UPPER_N_TITLE,
CHAR_UPPER_O_TITLE,
CHAR_UPPER_U_DIERESIS,
CHAR_UPPER_U_TITLE,
CHAR_XI_UPPER
};

#endif


