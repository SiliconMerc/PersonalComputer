# PersonalComputer
- ALU/CPU design and implementation using elementary logic gates and flip flops in HDLWriting
- Assembler , VMTranslator and Compiler to parse Java like high level language responsible
  for converting bytecode to machine-specific code

- Understanding Object-based design and programming, abstract data types, scoping rules, syntax and semantics, references

## The HACK Computer

![image-20220828162415935](./images/image-20220828162415935.png)

### THE ISA

![image-20220828162617873](./images/image-20220828162617873.png)

![image-20220828162710318](./images/image-20220828162710318.png)

![image-20220828162800039](./images/image-20220828162800039.png)

![image-20220828162817807](./images/image-20220828162817807.png)

![image-20220828162806366](./images/image-20220828162806366.png)

## The JACK Compiler

### Memory Segements

![image-20220828163053252](./images/image-20220828163053252.png)

![image-20220828163136023](./images/image-20220828163136023.png)

### Function Implementation

![image-20220828163250025](./images/image-20220828163250025.png)

![image-20220828163257863](./images/image-20220828163257863.png)

### The JACK Tokenizer

![image-20220828163538187](./images/image-20220828163538187.png)



### Compiling Epxressions

![image-20220828163616283](./images/image-20220828163616283.png)



### Crafting the Compiler

http://www.craftinginterpreters.com/contents.html

#### Symbol Tables

![image-20220828163742908](./images/image-20220828163742908.png)

![image-20220828163754149](./images/image-20220828163754149.png)

![image-20220828163801873](./images/image-20220828163801873.png)

### Objetct Construction 

![image-20220828164045063](./images/image-20220828164045063.png)

1. A constructor allocates memory for the object on the memory heap based on the number of fields and returns the reference to the start of memory segment allocated fot this object
2. For further calls we use the **this** pointer(containing the memory address of start of class) to access the variables stored for that class 

Array Implementation

![image-20220828164527729](./images/image-20220828164527729.png)

- We use the **that** pointer for this approach , it stores the address of the location from which we want to read/write the data  
- We push the data at that = arr+expression1  

### The OS

![image-20220828163431105](./images/image-20220828163431105.png)

Heap Management

![image-20220828165012033](./images/image-20220828165012033.png)

![image-20220828165020918](./images/image-20220828165020918.png)

The Big Picture

![image-20220828163336325](./images/image-20220828163336325.png)

![image-20220828165059901](./images/image-20220828165059901.png)



**Exposure: Computer Architecture , Machine Language , Compiler Design , Operating System**
