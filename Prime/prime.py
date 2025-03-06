# {x∈N∣x>2∧∀d∈N:(d∣x⇒d=1∨d=x)}
# TODO: fix user input another time
def natural_number(x):
    return isinstance(x, int) and x > 0

def greater_than_2(x):
    return x > 2

def implication(x, d):
    divides = (x % d == 0)
    check = (d == 1 or d == x)
    return (not divides) or check

def check_all_divisors(x):
    for d in range(1 , x + 1):
        if not implication(x, d): 
            return False
    return True

def is_prime(x):
    if not natural_number(x): return False
    if not greater_than_2(x): return False
    return check_all_divisors(x)

while True:
    x = int(input("Enter a number and I will check if it is prime: "))
    result = is_prime(x)
    print(f"{x} is prime" if result else f"{x} is composite")



    
