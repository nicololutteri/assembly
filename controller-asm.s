.data
.text
.global controllerasm

LessThan60:	
		movb $65, %bl; # Move A
		jmp PrintNCK; 
		
GreterThan80: 
		movb $66, %bl; # Move B
		jmp PrintNCK; 

GreterThan802:
		movb 0x6(%esi), %al; # Move third
		cmp $48, %al; # if '0' jump, else not
		je Neutral;
		jmp GreterThan80;

controllerasm:
		movl 4(%esp), %esi; # Prepare puntator bufferin
		movl 8(%esp), %edi; # Preapre puntator bufferout

		pushl %ebp; # Save ebp for using later
		
		xorw %bp, %bp; # Delete bp
		movw $12336, %cx; # Prepare NCK 48 '0' - 48 '0'
	
EncodePH:
		movl $738197548, 0x1(%edi); # Save comma inside bufferout
		
		movl 0x5(%esi), %eax; # Read for comparson

		movb (%esi), %dl; # Continue if init is not 0, jump if init si 1
		cmpb $48, %dl; 
		je EndResetInit; 
				
		movb 0x2(%esi), %dl; # Continue if reset is not equal to 1, jump if it is 1
		cmpb $49, %dl; 
		je EndResetInit; 

		movw 0x4(%esi), %ax; # Read pH
		
		cmpb $49, %al;  # Compare * 100 part of pH
		je GreterThan80;
		cmpb $54, %ah; # Compare * 10 part of pH with 6
		jl LessThan60; 
		cmpb $56, %ah; # Compare * 10 part of pH with 8
		jg GreterThan80; 
		je GreterThan802; 
		
Neutral:
		movb $78, %bl; # Move N
		
PrintNCK: 
		movb %bl, (%edi); # Write the value of pH in the memory
		cmpw %bx, %bp;  
		mov $12336, %edx; # Use cmovne to not use the jump
		cmovne %edx, %ecx; # Delete NCK if the pH is changed
		
		movw %bx, %bp; # Save the current ebp

		movw %cx, 0x2(%edi);
				
		cmpb $49, %cl; # Check if NCK => 10
		jge Equals;
		cmpb $53, %ch; # Check if NCK => 5
		jge Equals;
		
NotEquals: 
		movw $11565, 0x5(%edi);  # Write -- su bufferout
		jmp End; 
	
Equals: 
		cmpb $65, %bl; # Compare if bl contains 65 'A' 
		je WriteBS; 
		cmpb $66, %bl; # Compare if bl contains 66 'B'
		je WriteAS; 
Monomioc
		movw $11565, 0x5(%edi);  # Write -- su bufferout
		jmp End; 

EndResetInit: 
		movl $757935149, (%edi); # Write -,--
		movb $44, 0x4(%edi); # Write ,
		movw $11565, 0x5(%edi); # Write --

		xorw %bp, %bp; # Delete pH
		movw $12336, %cx; # Restore NCK
		
		jmp End; 
		
WriteAS:  
		movw $21313, 0x5(%edi); # Write AS on memory
		jmp End; 
		
WriteBS:  
		movw $21314, 0x5(%edi); # Write BS on memory
		
End: 
		movb $10, 0x7(%edi); # \r

		addl $8, %esi; # Move esi index (bufferin)
		addl $8, %edi; # Move edi index (bufferout)
		
		movb (%esi), %dl;
		testb %dl, %dl; # Check End strin
		jz EndASM; 
		
		cmpb $57, %ch; # Check if NCK is not * - '9', otherwise i have to add another digit
		je AddNCK;

		incb %ch; # Add 1 to NCK
		jmp EncodePH;

AddNCK:
		incb %cl; # Add 1 in the * 10 position
		movb $48, %ch;
		jmp EncodePH;
		
EndASM: 
		popl %ebp; # Restore the pointer at the base of the stack
		ret # Return to C
