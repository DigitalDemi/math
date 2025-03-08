# {x∈N∣x>2∧∀d∈N:(d∣x⇒d=1∨d=x)}
# TODO: fix user input another time
# TODO: check for the optimisation for checking quickly if prime

class Prime:
    def __init__(self, x):
        self.x = x
        
    
    def __repr__(self):
        return f"Prime({self.x})"
    
    def __str__(self):
        if self.is_prime(): return f"{self.x} is a prime number"
        else: return f"{self.x} is a composite"


    def natural_number(self):
        return isinstance(self.x, int) and self.x > 0

    def greater_than_2(self):
        return self.x > 2

    def implication(self,d):
        divides = (self.x % d == 0)
        check = (d == 1 or d == self.x)
        return (not divides) or check

    def check_all_divisors(self):
        for d in range(1 , self.x + 1):
            if not self.implication(d): return False
        return True

    def is_prime(self):
        if not self.natural_number(): return False
        if not self.greater_than_2(): return False
        return self.check_all_divisors()

# while True:
#     x = int(input("Enter a number and I will check if it is prime: "))
#     result = is_prime(x)
#     print(f"{x} is prime" if result else f"{x} is composite")