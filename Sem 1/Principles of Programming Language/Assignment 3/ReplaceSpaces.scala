import scala.io.StdIn
object ReplaceSpaces{
  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine()

    val result = str.replace(' ', '-')

    println("Modified String = " + result)
  }
}