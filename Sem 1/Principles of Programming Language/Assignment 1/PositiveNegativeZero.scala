import scala.io.StdIn
object PositiveNegativeZero{
  def main(args: Array[String]): Unit ={
    print("Enter a number: ")
    val num = StdIn.readInt()

    if(num > 0)
      println("Positive Number")
    else if(num < 0)
      println("Negative Number")
    else
      println("Zero")
  }
}