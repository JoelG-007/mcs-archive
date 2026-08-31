import scala.io.StdIn
object PalindromeString{
  def checkPalindrome(str: String): String ={
    val rev = str.reverse
    (str == rev) match{
      case true => "Palindrome"
      case false => "Not a Palindrome"
    }
  }

  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine()

    if(str.nonEmpty)
      println(checkPalindrome(str))
    else
      println("Empty String")
  }
}