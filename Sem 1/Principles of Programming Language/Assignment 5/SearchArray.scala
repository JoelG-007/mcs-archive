import scala.io.StdIn
object SearchArray{
  def main(args: Array[String]): Unit ={
    val arr = new Array[Int](5)
    println("Enter 5 elements:")

    for(i <- 0 until arr.length)
      arr(i) = StdIn.readInt()

    print("Enter element to search: ")
    val key = StdIn.readInt()

    var found = false

    for(i <- 0 until arr.length){
      if(arr(i) == key){
        println("Element found at index " + i)
        found = true
      }
    }
    if(!found)
      println("Element not found")
  }
}