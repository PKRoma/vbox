
/* DO NOT EDIT - THIS FILE IS AUTOMATICALLY GENERATED FROM THE .syn FILE */

".syntax expression;\n"
".emtcode EXP_END 0\n"
".emtcode EXP_EXPRESSION 1\n"
".emtcode OP_END 0\n"
".emtcode OP_PUSHINT 1\n"
".emtcode OP_LOGICALOR 2\n"
".emtcode OP_LOGICALAND 3\n"
".emtcode OP_OR 4\n"
".emtcode OP_XOR 5\n"
".emtcode OP_AND 6\n"
".emtcode OP_EQUAL 7\n"
".emtcode OP_NOTEQUAL 8\n"
".emtcode OP_LESSEQUAL 9\n"
".emtcode OP_GREATEREQUAL 10\n"
".emtcode OP_LESS 11\n"
".emtcode OP_GREATER 12\n"
".emtcode OP_LEFTSHIFT 13\n"
".emtcode OP_RIGHTSHIFT 14\n"
".emtcode OP_ADD 15\n"
".emtcode OP_SUBTRACT 16\n"
".emtcode OP_MULTIPLY 17\n"
".emtcode OP_DIVIDE 18\n"
".emtcode OP_MODULUS 19\n"
".emtcode OP_PLUS 20\n"
".emtcode OP_MINUS 21\n"
".emtcode OP_NEGATE 22\n"
".emtcode OP_COMPLEMENT 23\n"
"expression\n"
" first_expression .and optional_second_expression .and optional_space .and '\\0' .emit EXP_END;\n"
"first_expression\n"
" optional_space .and logical_or_expression .emit EXP_EXPRESSION .and .true .emit OP_END;\n"
"optional_second_expression\n"
" second_expression .or .true;\n"
"second_expression\n"
" space .and logical_or_expression .emit EXP_EXPRESSION .and .true .emit OP_END;\n"
"logical_or_expression\n"
" logical_and_expression .and .loop logical_or_expression_1;\n"
"logical_or_expression_1\n"
" barbar .and logical_and_expression .and .true .emit OP_LOGICALOR;\n"
"logical_and_expression\n"
" or_expression .and .loop logical_and_expression_1;\n"
"logical_and_expression_1\n"
" ampersandampersand .and or_expression .and .true .emit OP_LOGICALAND;\n"
"or_expression\n"
" xor_expression .and .loop or_expression_1;\n"
"or_expression_1\n"
" bar .and xor_expression .and .true .emit OP_OR;\n"
"xor_expression\n"
" and_expression .and .loop xor_expression_1;\n"
"xor_expression_1\n"
" caret .and and_expression .and .true .emit OP_XOR;\n"
"and_expression\n"
" equality_expression .and .loop and_expression_1;\n"
"and_expression_1\n"
" ampersand .and equality_expression .and .true .emit OP_AND;\n"
"equality_expression\n"
" relational_expression .and .loop equality_expression_1;\n"
"equality_expression_1\n"
" equality_expression_2 .or equality_expression_3;\n"
"equality_expression_2\n"
" equalsequals .and relational_expression .and .true .emit OP_EQUAL;\n"
"equality_expression_3\n"
" bangequals .and relational_expression .and .true .emit OP_NOTEQUAL;\n"
"relational_expression\n"
" shift_expression .and .loop relational_expression_1;\n"
"relational_expression_1\n"
" relational_expression_2 .or relational_expression_3 .or relational_expression_4 .or\n"
" relational_expression_5;\n"
"relational_expression_2\n"
" lessequals .and shift_expression .and .true .emit OP_LESSEQUAL;\n"
"relational_expression_3\n"
" greaterequals .and shift_expression .and .true .emit OP_GREATEREQUAL;\n"
"relational_expression_4\n"
" less .and shift_expression .and .true .emit OP_LESS;\n"
"relational_expression_5\n"
" greater .and shift_expression .and .true .emit OP_GREATER;\n"
"shift_expression\n"
" additive_expression .and .loop shift_expression_1;\n"
"shift_expression_1\n"
" shift_expression_2 .or shift_expression_3;\n"
"shift_expression_2\n"
" lessless .and additive_expression .and .true .emit OP_LEFTSHIFT;\n"
"shift_expression_3\n"
" greatergreater .and additive_expression .and .true .emit OP_RIGHTSHIFT;\n"
"additive_expression\n"
" multiplicative_expression .and .loop additive_expression_1;\n"
"additive_expression_1\n"
" additive_expression_2 .or additive_expression_3;\n"
"additive_expression_2\n"
" plus .and multiplicative_expression .and .true .emit OP_ADD;\n"
"additive_expression_3\n"
" dash .and multiplicative_expression .and .true .emit OP_SUBTRACT;\n"
"multiplicative_expression\n"
" unary_expression .and .loop multiplicative_expression_1;\n"
"multiplicative_expression_1\n"
" multiplicative_expression_2 .or multiplicative_expression_3 .or multiplicative_expression_4;\n"
"multiplicative_expression_2\n"
" star .and unary_expression .and .true .emit OP_MULTIPLY;\n"
"multiplicative_expression_3\n"
" slash .and unary_expression .and .true .emit OP_DIVIDE;\n"
"multiplicative_expression_4\n"
" percent .and unary_expression .and .true .emit OP_MODULUS;\n"
"unary_expression\n"
" primary_expression .or unary_expression_1 .or unary_expression_2 .or unary_expression_3 .or\n"
" unary_expression_4;\n"
"unary_expression_1\n"
" plus .and unary_expression .and .true .emit OP_PLUS;\n"
"unary_expression_2\n"
" dash .and unary_expression .and .true .emit OP_MINUS;\n"
"unary_expression_3\n"
" bang .and unary_expression .and .true .emit OP_NEGATE;\n"
"unary_expression_4\n"
" tilda .and unary_expression .and .true .emit OP_COMPLEMENT;\n"
"primary_expression\n"
" intconstant .or primary_expression_1;\n"
"primary_expression_1\n"
" lparen .and logical_or_expression .and rparen;\n"
"intconstant\n"
" integer .emit OP_PUSHINT;\n"
"integer\n"
" integer_dec;\n"
"integer_dec\n"
" digit_dec .emit 10 .emit * .and .loop digit_dec .emit * .and .true .emit '\\0';\n"
"digit_dec\n"
" '0'-'9';\n"
"optional_space\n"
" .loop single_space;\n"
"space\n"
" single_space .and .loop single_space;\n"
"single_space\n"
" ' ' .or '\\t';\n"
"ampersand\n"
" optional_space .and '&' .and optional_space;\n"
"ampersandampersand\n"
" optional_space .and '&' .and '&' .and optional_space;\n"
"bang\n"
" optional_space .and '!' .and optional_space;\n"
"bangequals\n"
" optional_space .and '!' .and '=' .and optional_space;\n"
"bar\n"
" optional_space .and '|' .and optional_space;\n"
"barbar\n"
" optional_space .and '|' .and '|' .and optional_space;\n"
"caret\n"
" optional_space .and '^' .and optional_space;\n"
"dash\n"
" optional_space .and '-' .and optional_space;\n"
"equalsequals\n"
" optional_space .and '=' .and '=' .and optional_space;\n"
"greater\n"
" optional_space .and '>' .and optional_space;\n"
"greaterequals\n"
" optional_space .and '>' .and '=' .and optional_space;\n"
"greatergreater\n"
" optional_space .and '>' .and '>' .and optional_space;\n"
"less\n"
" optional_space .and '<' .and optional_space;\n"
"lessequals\n"
" optional_space .and '<' .and '=' .and optional_space;\n"
"lessless\n"
" optional_space .and '<' .and '<' .and optional_space;\n"
"lparen\n"
" optional_space .and '(' .and optional_space;\n"
"percent\n"
" optional_space .and '%' .and optional_space;\n"
"plus\n"
" optional_space .and '+' .and optional_space;\n"
"rparen\n"
" optional_space .and ')' .and optional_space;\n"
"slash\n"
" optional_space .and '/' .and optional_space;\n"
"star\n"
" optional_space .and '*' .and optional_space;\n"
"tilda\n"
" optional_space .and '~' .and optional_space;\n"
""
