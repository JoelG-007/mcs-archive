object SumPrime {
  def isPrime(num: Int): Boolean ={
    if(num < 2)
      return false

    var i = 2

    while(i <= Math.sqrt(num).toInt){
      if (num % i == 0)
        return false
      i += 1
    }

    true
  }

  def main(args: Array[String]): Unit ={
    var sum = 0
    for (i <- 1 to 100) {
      if (isPrime(i))
        sum += i
    }

    println("Sum of Prime Numbers = " + sum)
  }
}