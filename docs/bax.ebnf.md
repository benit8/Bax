# Bax's syntax

```ebnf
top-level-statement = namespace-statement
                    | class-declaration;

namespace-statement = "namespace", namespaced-identifier;

declaration = class-declaration;

block-statement = '{', { statement|declaration|expression-statement }, '}';

expression-statement = ( assignment-expression | function-call-expression ), ';';

statement = block-statement | if-statement | match-statement | return-statement | while-statement;

if-statement = "if", expression, statement;

match-statement = "match", '(', expression, ')', '{', { match-case }, '}';
match-case = { default | { expression }, '=>', expression };

return-statement = "return", expression;

while-statement = "while", '(', boolean-expression, ')', statement;

expression = array-expression
           | assignment-expression
           | binary-expression
           | call-expression
           | function-expression
           | match-expression
           | member-expression
           | object-expression
           | subscript-expression
           | ternary-expression
           | unary-expression
           | update-expression
           | variable-identifier;

namespaced-indentifier = studly-case-identifier, { '\', studly-case-identifier };

studly-case-identifier = uppercase-alphabetic-character, { alpha-numeric-character };
camel-case-identifier = lowercase-alphabetic-character, { alpha-numeric-character };
snake-case-identifier = lowercase-alphabetic-character, { lowercase-alpha-numeric-character | '_' };
kebab-case-identifier = lowercase-alphabetic-character, { lowercase-alpha-numeric-character | '-' };
```
