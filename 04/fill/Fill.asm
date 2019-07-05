D = A [SCREEN]
D = D + A [8192]
M = D [t]
(INIT)
D = A [SCREEN]
M = D [p]  // go to the first 16 pixels of the screen
(LOOP)
D = M [p]
D = M - D [t]
@INIT
D; JEQ
D = M [24576]  // accept the kbd input
@WHITE
D; JEQ
// key pressed
A = M [p]
D = M
@REVERSE
D; JEQ
@NEXT
0; JMP
// no key pressed
(WHITE)
A = M [p]
D = M
@REVERSE
D; JNE
@NEXT
0; JMP
(REVERSE)
A = M [p]
M = !M
(NEXT)
M = M + 1 [p]
@LOOP
0; JMP