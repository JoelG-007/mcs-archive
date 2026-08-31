import scala.io.StdIn
object ReverseWords{
  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine()
    val words = str.split(" ")
    var result = ""

    for(i <- 0 until words.length){
      var rev = ""
      var j = words(i).length - 1

      while(j >= 0){
        rev += words(i).charAt(j)
        j -= 1
      }

      result += rev

      if(i != words.length - 1)
        result += " "
    }
    println("Reversed Words: " + result)
  }
}