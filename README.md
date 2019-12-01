# simple-Java-interpreter

Simple interpreter for subset of Java programming language

Uses recursive descent parser for syntax analyzing and RPN (Reverse Polish notation) for executing program


## Supported features:

- comparisons (==, >, <, >=, <=, !=)
- inc and dec (++, --)
- arithmetic expressions (-, +, /, %)
- logical expressions (!, &&, ||)
- string, boolean and int variables
- while and if (with else) constructions
- printing variables and literals sequences (System.out.println("line"))
- blocks ({})

Warning: variables are not allowed to be initialized.


## Code example:

```C++
public class MyClassName
{
	public static void main() 
	{
		int i;
		i = 0;
		boolean flag;
		flag = true;
		string notif;
		notif = "Finish\t this\n";
		while (i++ < 6 || flag)
		{
			System.out.println("Hmm.. ");
			if((i % 2 == 0) && (i >= 5))
			{
				System.out.println(notif);
				flag = false;
			}
			else
				System.out.println("OK.\n");
     		}
  	 }
}
```
