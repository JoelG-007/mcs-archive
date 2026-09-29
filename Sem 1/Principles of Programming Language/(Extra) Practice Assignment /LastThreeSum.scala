object LastThreeSum {
  def main(args: Array[String]): Unit ={
    val arr = Array(10, 20, 30, 40, 50)

    val result =
      if (arr.isEmpty)
        0
      else if (arr.length < 3)
        arr.sum
      else
        arr.takeRight(3).sum

    println("Result = " + result)
  }
}