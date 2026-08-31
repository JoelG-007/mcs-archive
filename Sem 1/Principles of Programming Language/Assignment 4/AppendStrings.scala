import scala.io.StdIn
object AppendStrings{
  def main(args: Array[String]): Unit ={
    print("Enter first string: ")
    var str1 = StdIn.readLine()
    print("Enter second string: ")
    var str2 = StdIn.readLine()

    if(str1.length > str2.length)
      str1 = str1.drop(str1.length - str2.length)
    else if(str2.length > str1.length)
      str2 = str2.drop(str2.length - str1.length)

    val result = str1 + str2
    println("Result = " + result)
  }
}