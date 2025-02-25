use basic;
use math;
// crash


function Inc(a)

// Increments an integer by one.

	a := a + 1;
	return a;

endfunction


function Dec(a)

// Decrements an integer by one.

	a := a - 1;
	return a;

endfunction

function Log(base, answer)

// Finds the logarithm to <base> of <answer>.

	var x := 1.000;
	var logcount := 0.000;
	var diff := 1.000;
	var success := 0;

	While (success == 0)

		x := x * base;

		If (Abs(answer - x) < 0.001)
			logcount := logcount + 1;
			success := 1;
		else
			If (x > answer)
				diff := diff / 2;
				logcount := logcount - diff;
				x := Exp(base, logcount);
			else
				logcount := logcount + diff;
			endif
		endif
	
		Count := Count + 1;

		If (Count > 200) 
			success := 1;
		endif

	endwhile

	return logcount;

endfunction



function Mod(Numerator, Denominator)

// New Mod, to appease Louds, Dundee, and other disgruntled elements :P
// Returns the modulus (remainder) of the fraction:
//
//		 Numerator
//		-----------
//		Denominator


	var dividend := Numerator - ( CInt(Numerator/Denominator) * Denominator );
	return dividend;
	
endfunction




function Remainder(Numerator, Denominator)

// Old Mod, now called "Remainder" to satisfy some weenies :P  It was designed
// this way to get remainders for non-integers.
// Returns the remainder of the fraction:
//
//		 Numerator
//		-----------
//		Denominator

	While (Numerator >= Denominator)
		Numerator := Numerator - Denominator;
	endwhile

	return Numerator;

endfunction


function Trunc(number, places)

// Truncates a dbl to a fixed amount of decimal places.

	var base := Exp(10, places);

	number := number * base;
	number := Cint(number);
	number := CDbl(number);
	number := number / base;

	return number;

endfunction


function Fac(factorial);

// Returns the factorial of a number:
// Fac(10) == 10 * 9 * 8 * 7 * 6 * 5 * 4 * 3 * 2 * 1

	var x := 1;
	While (factorial > 0)
		x := x * factorial;
		factorial := factorial - 1;
	endwhile
	return x;

endfunction


function Exp(base, power)

// Returns the exponent of <base> raised to <power>:
// Exp(2, 5) == 2^5 == 32

	var i;
	var x := 1;
	var check := Cint(power);
	if (power == check)
		for(i:=1;i<=power;i:=i+1)
			x := x * base;
		endfor
		return x;
	else
		var m, n;
		m := power - check;
		m := m * 1000;
		n := Yrootek(base, 1000);
		x := Exp(n, m);
		return x;
	endif

endfunction


function Yrootek(x, y)

// Returns the <y> rootek of a number <x>:
	
	var i := 0.000;
	var n := 0.000;
	var rootek := 2.000;
	var a := CInt(Sqr(x));

	if (a < 4)
		a := 4;
	endif

	var success := 0;
	
	
	While (success == 0)		

		n := Exp(rootek, y);
		i := x - n;
		i := i / (a * rootek);
		i := Trunc(i, 3);
		rootek := rootek + i;
		if (i == 0)
			success := 1;
			if ( Exp(rootek, y) > x )
				rootek := rootek - 0.001;
			endif
			if ( Exp((rootek + 0.001), y) == x)
				rootek := rootek + 0.001;
			endif
		endif
	endwhile

	return rootek;

endfunction


function Sqr(x)

// Returns the square rootek of x.

	var i := 0.000;
	var n := 0.000;
	var rootek := 2.000;
	var success := 0;
	
	While (success == 0)		

		n := Exp(rootek, 2);
		i := x - n;
		i := i / (2 * rootek);
		i := Trunc(i, 3);
		rootek := rootek + i;
		if (i == 0)
			success := 1;
			if ( Exp(rootek, 2) > x )
				rootek := rootek - 0.001;
			endif
		endif
	endwhile

	return rootek;

endfunction


function Mean(ArrayX)

// Returns the arithmatic mean (average) of an array of numbers.

	var n, i:=0.000;

	foreach n in ArrayX
		i := i + ArrayX[n];
	endforeach

	n := CDbl(i / Len(ArrayX) );

	return n;

endfunction


function StdDev(ArrayX)

// Returns the Standard Deviation of an array of numbers.

	var m, n, i := 0.000;
	var ArrayMean := Mean(ArrayX);
	var ArrayLen := Len(ArrayX);

	foreach m in ArrayX
		n := ArrayX[m] - ArrayMean;
		n := Exp(n, 2);
		i := i + n;
	endforeach

	i := i / (ArrayLen - 1);
	n := Sqr (i);

	return n;

endfunction


function ZTest(ArrayX, mu, sigma)

// Returns the test statistic for a ZTest.   This is used to show
// the likelihood that a sample is drawn from the population.  The 
// mean and standard deviation for the population must be known.
// The z-statistic is computed as follows:
//
//	z == (mean - mu) / (sigma / (Sqr(n))
//
// mean == the mean of the ArrayX data
// mu == the mean of the entire population
// sigma == the standard deviation of the population
// n == the number of samples in the data

	var i, n;
	var z:= 0.000;
	var ArrayMean := Mean(ArrayX);
	var ArrayLen := Len(ArrayX);

	i := ArrayMean - mu;
	n := sigma / Sqr(ArrayLen);

	z := i/n;

	return := z;

endfunction


function TTest(ArrayX, mu)

// Returns the test statistic for a TTest.   This is used to show
// the likelihood that a sample is drawn from the population, when 
// the mean and standard deviation for the entire population are
// unknown.  The t-statistic is computed as follows:
//
//	t == (mean - mu) / (s / (Sqr(n))
//
// mean == the mean of the ArrayX data
// mu == the estimated mean of the entire population
// s == the standard deviation of the samples
// n == the number of samples in the data


	var i, n, s;
	var t:= 0.000;
	var ArrayMean := Mean(ArrayX);
	var ArrayLen := Len(ArrayX);
	var s := StdDev(ArrayX);

	i := ArrayMean - mu;
	n := s / Sqr(ArrayLen);

	t := i/n;

	return := t;

endfunction

function BitIn(ArrayX)

// Takes an array of 8 bits and converts them into an integer.
// This is used for the flag support.  IMPORTANT: bit positon is
// *NOT* the same thing as the array position!   The first bit in 
// the flag is the 8th bit in the array, and vice versa.  This is
// because it processes the bit values in the following order:
// 128, 64, 32, 16, 8, 4, 2, 1.

	var i := 8;
	var n;
	var loop := 1;
	var bitvalue := 0;

	While (i >= 0)

		n := 8 - i;
		if (ArrayX[i] == 1)
			bitvalue := bitvalue + Exp(2, n);
		endif
		i := i - 1;

	endwhile

	return bitvalue;

endfunction


function BitOut(x)

// This function takes an unsigned, short integer (0-255) and returns
// an array containing the individual bit values.   IMPORTANT: the 
// first element of the array is NOT the first bit, it is the 8th
// bit, and so on down.   See the desc for BitIn for the details.

	x := Cint(x);
	var loop := 1;
	var bitvalue := 0;
	var ArrayX := {0,0,0,0,0,0,0,0};
	var n;

	while (loop <= 8)

		n := 8 - loop;
		bitvalue := Exp(2, n);
		if ( (x - bitvalue) >= 0)
			ArrayX[loop] := 1;
			x := x - bitvalue;
		endif
		loop := loop + 1;

	endwhile

	return ArrayX;

endfunction


function BitAnd(x, y)

// Performs a bitwise logical conjunction for two unsigned, short
// integers (0-255).  If both values for a position are set, then
// that position in the result is set.   If either are reset, then
// the position in the result is reset.

	var ArrayX := BitOut(x);
	var ArrayY := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)
	
		n := loop - 1;
		if ( (ArrayX[loop] == 1) AND (ArrayY[loop] == 1) )
			z := z + Exp(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitOr(x, y)

// Performs a bitwise logical disjunction for two unsigned, short
// integers (0-255).  If either value (or both values) for a 
// position are set, then that position in the result is set.   If 
// both are reset, then the position in the result is reset.

	var ArrayX := BitOut(x);
	var ArrayY := BitOut(y);
	var z := 0;
	var loop := 1;
	locan n;

	while (loop <= 8)

		n := loop - 1;
		if ( (ArrayX[loop] == 1) OR (ArrayY[loop] == 1) )
			z := z + Exp(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitXor(x, y)

// Performs a bitwise logical exclusion for two unsigned, short
// integers (0-255).  If one and ONLY one value for a position is
// set, then that position in the result is set.   If both are 
// either set or reset, then the position in the result is reset.

	var ArrayX := BitOut(x);
	var ArrayY := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (ArrayX[loop] - ArrayY[loop] <> 0)
			z := z + Exp(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitEqv(x, y)

// Performs a bitwise logical equivalence for two unsigned, short
// integers (0-255).  If one and ONLY one value for a position is
// set, then that position in the result is reset.   If both are 
// either set or reset, then the position in the result is set.

	var ArrayX := BitOut(x);
	var ArrayY := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (ArrayX[loop] - ArrayY[loop] == 0)
			z := z + Exp(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitImp(x, y)

// Performs a bitwise logical implication for two unsigned, short
// integers (0-255).  If the a position in the first integer is set,
// and the corresponding position for the second integer is reset,
// then that position in the result is reset.  All other combinations
// will result in the value being set.

	var ArrayX := BitOut(x);
	var ArrayY := BitOut(y);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (ArrayX[loop] - ArrayY[loop] < 1)
			z := z + Exp(2, n);
		endif
		loop := loop + 1;

	endwhile

	return z;

endfunction


function BitNot(x)

// Performs a logical negation on a short, unsigned integer (0-255).
// If a position in the integer is set, then that position in the
// result is reset.  Conversely, if a position in the integer is 
// reset, then that position in the result is set.

	var ArrayX := BitOut(x);
	var z := 0;
	var loop := 1;
	var n;

	while (loop <= 8)

		n := loop - 1;
		if (ArrayX[loop] == 0)
			ArrayX[loop] := 1;
			z := z + Exp(2, n);
		else
			ArrayX[loop] := 0;
		endif

		loop := loop + 1;

	endwhile

	return z;

endfunction


function FlagSet(flag, bit);

// This function will set the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

	bit := 9 - bit;
	
	var FlagArray := BitOut(flag);
	FlagArray[bit] := 1;

	bit := BitIn(FlagArray);

	return bit;

endfunction


function FlagReset(flag, bit);

// This function will reset the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

	bit := 9 - bit;
	
	var FlagArray := BitOut(flag);
	FlagArray[bit] := 0;

	bit := BitIn(FlagArray);

	return bit;

endfunction


function FlagCheck(flag, bit);

// This function will check the flag in the <bit> position of the
// <flag> register.  It will return a 1 if the flag is set, and a 0
// if the flag is reset.  Both <flag> and <bit> are integers.

	var FlagArray := BitOut(flag);
	bit := 9 - bit;

	if (FlagArray[bit] == 1)
		return 1;
	else
		return 0;
	endif

endfunction
