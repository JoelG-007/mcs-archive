import scala.io.StdIn
object NumberConversion{
  def toBinary(num: Int): String ={
    Integer.toBinaryString(num)
  }

  def toOctal(num: Int): String ={
    Integer.toOctalString(num)
  }

  def main(args: Array[String]): Unit ={
    print("Enter a number: ")
    val num = StdIn.readInt()

    println("Binary = " + toBinary(num))
    println("Octal = " + toOctal(num))
  }
}