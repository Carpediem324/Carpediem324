def gcd1(a,b):
    alist = []
    blist =[]
    for i in range(1,a):
        if a%i ==0 :
            alist.append(i)
    for i in range(1,b):
        if b%i ==0 :
            blist.append(i)
    print(a,"의 약수 = ",alist)
    print(b,"의 약수 = ",blist)

    for i in range(len(alist)-1,0,-1):
        if alist[i] in blist:
            return print(a,"과",b,"의 최대공약수 =",alist[i])
    



def gcd2(a,b):
    alist = []
    for i in range(1,a):
        if a%i ==0 :
            alist.append(i)
    print(a,"의 약수 = ",alist)


    for i in range(len(alist)-1,0,-1):
        if b % alist[i]==0:
            return print(a,"과",b,"의 최대공약수 =",alist[i])
    



def gcd3(a,b) :
  print("gcd(",a,b,")")
  if(a<b):
    print(a,"는",b,"보다 작습니다. a와 b를 교체합니다")
    x=a
    a=b
    b=x
  firsta=a
  firstb=b
  print("gcd(",a,b,")")
  while b !=0 :
    r = a%b
    a = b
    b = r
    print("gcd(",a,b,")")
  return print(firsta,"과",firstb,"의 최대공약수 =",a)

def lcm(a,b) :
  print("lcd(",a,b,")")
  for i in range(max(a,b),(a*b)+1):
    if i%a==0 and i%b==0 :
      return print(a,"과",b,"의 최소공배수 =",i)


gcd1(60,28)
print()
gcd1(50,28)
print()
gcd1(40,28)
print()
gcd2(60,28)
print()
gcd2(50,28)
print()
gcd2(40,28)
print()
gcd3(28,60)
print()
gcd3(28,50)
print()
gcd3(28,40)
print()
lcm(2,3)
print()
lcm(2,4)
print()
lcm(2,5)
print()
