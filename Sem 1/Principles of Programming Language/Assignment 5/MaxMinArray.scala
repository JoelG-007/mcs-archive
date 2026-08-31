import scala.io.StdIn
object MaxMinArray{
  def main(args: Array[String]): Unit ={
    val arr = new Array[Int](5)

    println("Enter 5 elements:")

    for(i <- 0 until arr.length)
      arr(i) = StdIn.readInt()

    var max = arr(0)
    var min = arr(0)

    for(i <- 1 until arr.length){

      if(arr(i) > max)
        max = arr(i)

      if(arr(i) < min)
        min = arr(i)
    }

    println("Maximum = " + max)
    println("Minimum = " + min)
  }
}