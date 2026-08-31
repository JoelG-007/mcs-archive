import scala.io.StdIn

object LeapYear{
    def main(args: Array[String]): Unit ={
    print("Enter a year: ")
    val year = StdIn.readInt()

    if((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
        println("Leap Year")
    else
        println("Not a Leap Year")
  }
}