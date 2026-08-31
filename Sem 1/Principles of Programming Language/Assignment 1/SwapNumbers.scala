import scala.io.StdIn
object SwapNumbers{
  def main(args: Array[String]): Unit ={
    print("Enter first number: ")
    var a = StdIn.readInt()
    print("Enter second number: ")
    var b = StdIn.readInt()

    println("Before Swapping")
    println("a = " + a)
    println("b = " + b)

    a = a + b
    b = a - b
    a = a - b

    println("After Swapping")
    println("a = " + a)
    println("b = " + b)
  }
}