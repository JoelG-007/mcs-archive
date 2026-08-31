import scala.io.StdIn
object RemoveCharacter{
  def main(args: Array[String]): Unit ={
    print("Enter a string: ")
    val str = StdIn.readLine()
    print("Enter the position to remove: ")
    val pos = StdIn.readInt()

    if(pos >= 0 && pos < str.length){
      val result = str.take(pos) + str.drop(pos + 1)
      println("New String = " + result)
    }else{
      println("Invalid Position")
    }
  }
}