import scala.io.StdIn
object Factorial{
  def main(args: Array[String]): Unit ={
    print("Enter a number: ")
    val num = StdIn.readInt()

    var fact = 1
    var i = 1

    while(i <= num){
      fact = fact * i
      i += 1
    }
    println("Factorial = " + fact)
  }
}