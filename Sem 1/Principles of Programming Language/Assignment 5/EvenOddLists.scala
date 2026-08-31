import scala.io.StdIn
object EvenOddLists{
  def main(args: Array[String]): Unit ={
    val arr = new Array[Int](5)

    println("Enter 5 numbers:")

    for(i <- 0 until arr.length)
      arr(i) = StdIn.readInt()

    var evenList = List[Int]()
    var oddList = List[Int]()

    for(num <- arr){
      if(num % 2 == 0)
        evenList = evenList :+ num
      else
        oddList = oddList :+ num
    }

    println("Even List: " + evenList)
    println("Odd List: " + oddList)
  }
}