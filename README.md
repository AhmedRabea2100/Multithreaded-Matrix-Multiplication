# Multithreaded-Matrix-Multiplication
Matrix multiplication using different number of threads
# How to compile and run
User should run the command `make` in the terminal which will generate anexecutable file `matMultp` then type `./matMultp` in the terminal to run the program.  
User can specify input and output files names’ or simply press Enter for using the default names.
# Design Decisions
input matrices are given as text files in form of 'a.txt' and 'b.txt'.
each one of the 3 methods produce a txt file in form of `c_number_of_threads` (e.g c_per_matrix).
# Code Organization
Code is divided into 2 main sections.  
First is reading the 2 input matrices and parsing them according to `“ “` and `“=”`.  
Second is performing the 3 types of multiplication in which each type is divided into 2 functions one responsible for creation of threads and the other for performing the multiplication.

![image](https://user-images.githubusercontent.com/95850756/186199417-f82f0735-27c2-4d91-b77f-55028e66541f.png)
# Main Functions.
### Parsers  
Parsing the given string according to “ “ or “=”.  

![image](https://user-images.githubusercontent.com/95850756/186199688-5a332f0c-559a-4671-aa0c-ca409e897b31.png)  
---
### Thread per matrix multiplication
![image](https://user-images.githubusercontent.com/95850756/186200485-af8cca12-d42b-4cb7-a52d-41a6f58fb8fc.png)
---
### Thread per row multiplication
![image](https://user-images.githubusercontent.com/95850756/186201508-5f484fe4-b510-48da-8fc6-07f345b72a2b.png)
---
### Thread per element multiplication
![image](https://user-images.githubusercontent.com/95850756/186201620-1a53fd3b-68a0-4a8d-9c10-dea4f0a3c566.png)
---
### Writing answers in files
![image](https://user-images.githubusercontent.com/95850756/186201779-9530438a-f5c6-47f1-93fd-9d9c25b1dbaa.png)

# Speed Comparison
![image](https://user-images.githubusercontent.com/95850756/186202395-162152c5-1f65-48f7-bb56-7ccbf26ef53c.png)

# Conclusion
Thread per element is the one which takes the greatest time as creating many threads is very time consuming which means that multi-threading is not always the
ideal solution.

