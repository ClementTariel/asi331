from hashlib import sha256
N = 24
M0 = "ALICE CERTIFIE QU ELLE DOIT NEUF EUROS ET TRENTE CENTIMES A BOB".replace(" ","")
M1 = "ALICE CERTIFIE QU ELLE DOIT DIX MILLE DEUX CENTS EUROS ET TRENTE CENTIMES A BOB".replace(" ","")

def hash_of(m):
	return int(sha256(str(m).encode()).hexdigest()[:N // 4], 16)

# q2)
def floyd(f,x_0,print_i=False):
	x_i = f(x_0)
	x_2i = f(x_i)
	i = 1
	while (x_i != x_2i):
		x_i = f(x_i)
		x_2i = f(f(x_2i))
		i += 1
	if print_i:
		print("collision pour i = ",i)
	return x_i

# q3)
def find_period(f,x_i):
	# find x_i = x_j and return j-i
	# j-i is not always the smallest period
	x_j = f(x_i)
	l = 1
	while (x_j != x_i):
		x_j = f(x_j)
		l += 1
	return l

def comp(f,n,x_0):
	# return f(f(...f(x_0))) with n times f
	x_n = x_0
	for i in range(n):
		x_n = f(x_n)
	return x_n

def int_to_bin(n):
	res = []
	while n>0:
		res.append(n%2==1)
		n = n//2
	return res

def g(n):
	n_bin = int_to_bin(n)
	if len(n_bin)==N: #b0 = 1
		M = M1
	else: #b0 = 0
		M = M0
		# complete the binary of n with zeros
		for i in range(N-len(n_bin)):
			n_bin.append(False)
	for i in range(N-1):
		if n_bin[N-2-i]:
			M = M[:i]+M[i].lower()+M[i+1:]
	return M

def h_g(n):
	return hash_of(g(n))


# executed only if the script is directly launched, not when it is just imported
if __name__ == "__main__":
	x_0 = 0

	#
	# q2)
	#
	print("\nquestion 2")
	x_i = floyd(hash_of,x_0)
	print("Il y a une collision pour x_i = x_2i = ",x_i)

	#
	# q3)
	#
	print("\nquestion 3")
	l = find_period(hash_of,x_i)
	print("l = ",l)

	#
	# q4)
	#
	print("\nquestion 4")
	x_i = x_0
	# l is used here 
	# x_0 chosen such that comp(hash_of,l,x_0) != x_0
	x_i_plus_l = comp(hash_of,l,x_0) 
	if (x_i == x_i_plus_l):
		print("ERROR ! x_0 is not suitable for this question.")
	else:
		print(x_i," != ",x_i_plus_l,", everything is fine.")
	while(hash_of(x_i) != hash_of(x_i_plus_l)):
		x_i = hash_of(x_i)
		x_i_plus_l = hash_of(x_i_plus_l)
	print("x_i = ",x_i)
	print("x_i+l = ",x_i_plus_l)
	print("h(x_i) = ",hash_of(x_i))
	print("h(x_i+l) = ",hash_of(x_i_plus_l))

	#
	# q5)
	#
	print("\nquestion 5")
	print("M0 = ",M0)
	print("M1 = ",M1)

	collision_found = False
	k = 0
	while not collision_found:
		k += 1
		x_0 = hash_of(k)
		x_i = floyd(h_g,x_0)
		l = find_period(h_g,x_i)
		x_i = x_0
		x_i_plus_l = comp(h_g,l,x_0) 
		if (x_i == x_i_plus_l):
			continue
		while(h_g(x_i) != h_g(x_i_plus_l)):
			x_i = h_g(x_i)
			x_i_plus_l = h_g(x_i_plus_l)
		collision_found = (g(x_i).lower() != g(x_i_plus_l).lower())
	print("x_i = ",x_i)
	print("x_i+l = ",x_i_plus_l)
	print("h(x_i) = ",h_g(x_i))
	print("h(x_i+l) = ",h_g(x_i_plus_l))
	print("M_i = ",g(x_i))
	print("M_i+l = ",g(x_i_plus_l))
	print("h(M_i) = ",hash_of(g(x_i)))
	print("h(M_i+l) = ",hash_of(g(x_i_plus_l)))