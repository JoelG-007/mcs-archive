object ArraySum {
  def main(args: Array[String]): Unit ={
    val arr = Array(10, 20, 30, 40, 50)

    // Using sum()
    println("Sum using sum(): " + arr.sum)

    // Using for loop
    var sum = 0
    for (i <- arr){
      sum += i
    }

    println("Sum using for loop: " + sum)
  }
}