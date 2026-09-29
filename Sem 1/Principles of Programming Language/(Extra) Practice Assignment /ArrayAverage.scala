object ArrayAverage {
  def main(args: Array[String]): Unit ={
    print("Enter number of elements: ")
    val n = scala.io.StdIn.readInt()
    val arr = new Array[Int](n)

    println("Enter array elements:")
    for (i <- 0 until n){
      arr(i) = scala.io.StdIn.readInt()
    }

    val sum = arr.sum
    val average = sum.toDouble / n

    println("Average = " + average)
  }
}