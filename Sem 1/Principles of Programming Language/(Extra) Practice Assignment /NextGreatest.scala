object NextGreatest {
  def main(args: Array[String]): Unit ={
    val arr = Array(58, 27, 100, 24, -5, 2, -6)

    for (i <- 0 until arr.length - 1){
      var max = arr(i + 1)

      for (j <- i + 1 until arr.length){
        if (arr(j) > max) {
          max = arr(j)
        }
      }

      arr(i) = max
    }

    arr(arr.length - 1) = -1

    println("Modified array:")
    println(arr.mkString(", "))
  }
}